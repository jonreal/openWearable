// openWearable web scope — vanilla + uPlot. Renders whatever dashboard the hub sends.
"use strict";

const panelsEl = document.getElementById("panels");
const statusEl = document.getElementById("status");
const titleEl = document.getElementById("title");
const controlsEl = document.getElementById("controls");
const armBtn = document.getElementById("arm");

const WINDOW = 2000; // samples kept per field (uPlot downsamples to screen width)
const PALETTE = ["#3b82f6", "#ef4444", "#10b981", "#f59e0b",
                 "#8b5cf6", "#ec4899", "#14b8a6", "#eab308"];

let fields = [];          // schema field names, in order
let xField = null;        // x-axis field, or null -> synthetic sample index
let buffers = {};         // field -> number[]
let sampleIdx = [];       // synthetic x when xField is null
let plots = [];           // [{u, fields}]
let plotsReady = false;
let controls = [];        // [{spec, el, valEl, echoEl}]
let armed = false;
let sock = null;          // active WebSocket (controls send through it)

function color(i) { return PALETTE[i % PALETTE.length]; }

function setStatus(connected) {
  statusEl.textContent = connected ? "connected" : "disconnected";
  statusEl.className = connected ? "connected" : "disconnected";
}

function setup(msg) {
  const dash = msg.dashboard;
  fields = msg.fields;
  xField = dash.x;
  titleEl.textContent = dash.title || "openWearable scope";
  document.title = titleEl.textContent;

  buffers = {};
  fields.forEach(f => (buffers[f] = []));
  sampleIdx = [];

  panelsEl.innerHTML = "";
  plots = [];

  for (const p of dash.panels) {
    const card = document.createElement("div");
    card.className = "panel";
    const h = document.createElement("div");
    h.className = "panel-title";
    h.textContent = p.title;
    card.appendChild(h);
    const body = document.createElement("div");
    card.appendChild(body);
    panelsEl.appendChild(card);

    const series = [{}].concat(p.fields.map((f, i) => {
      const s = { label: f, stroke: color(i), width: 1.5 };
      if (p.step) s.paths = uPlot.paths.stepped({ align: 1 });
      return s;
    }));

    const opts = {
      width: body.clientWidth || 900,
      height: 190,
      scales: { x: { time: false } },
      series,
      axes: [{}, { label: p.y || "" }],
      cursor: { drag: { x: true, y: false } },
      legend: { live: true },
    };
    const data = [[]].concat(p.fields.map(() => []));
    const u = new uPlot(opts, data, body);
    plots.push({ u, fields: p.fields });
  }
  plotsReady = true;
  buildControls(dash.controls || []);
  redraw();
}

// -- control panel (commands out; gated by a single ARM switch) --------------
function sendCmd(obj) {
  if (sock && sock.readyState === WebSocket.OPEN) sock.send(JSON.stringify(obj));
}

function setArmed(a) {
  armed = a;
  armBtn.textContent = a ? "ARMED" : "DISARMED";
  armBtn.className = a ? "armed" : "disarmed";
  controls.forEach(c => { c.el.disabled = !a; });   // controls live only while armed
}

function buildControls(specs) {
  controlsEl.innerHTML = "";
  controls = [];
  armBtn.hidden = specs.length === 0;
  controlsEl.hidden = specs.length === 0;
  for (const spec of specs) {
    const row = document.createElement("div");
    row.className = "ctl";
    const label = document.createElement("label");
    label.textContent = spec.label;
    row.appendChild(label);

    let el, valEl = null;
    if (spec.kind === "toggle") {
      el = document.createElement("input");
      el.type = "checkbox";
      el.addEventListener("change",
        () => sendCmd({ type: "cmd", name: spec.cmd, value: el.checked ? 1 : 0 }));
    } else {
      el = document.createElement("input");
      el.type = "range";
      el.min = spec.lo; el.max = spec.hi; el.step = spec.step; el.value = spec.lo;
      valEl = document.createElement("span");
      valEl.className = "ctl-val";
      valEl.textContent = (+spec.lo).toFixed(2);
      el.addEventListener("input", () => {
        valEl.textContent = (+el.value).toFixed(2);
        sendCmd({ type: "cmd", name: spec.cmd, value: +el.value });
      });
    }
    el.disabled = true;                 // gated until armed
    row.appendChild(el);
    if (valEl) row.appendChild(valEl);
    const echoEl = document.createElement("span");
    echoEl.className = "ctl-echo";
    row.appendChild(echoEl);
    controlsEl.appendChild(row);
    controls.push({ spec, el, valEl, echoEl });
  }
  armBtn.onclick = () => sendCmd({ type: "arm", on: !armed });
  setArmed(false);
}

function latest(field) {
  const b = buffers[field];
  return (b && b.length) ? b[b.length - 1] : null;
}

function updateControls() {
  const a = latest("armed");
  if (a !== null) setArmed(a >= 0.5);
  for (const c of controls) {
    if (!c.spec.echo) continue;
    const v = latest(c.spec.echo);
    if (v === null) continue;
    if (c.spec.kind === "toggle") {
      if (document.activeElement !== c.el) c.el.checked = v >= 0.5;  // don't fight the user
      c.echoEl.textContent = v >= 0.5 ? "on" : "off";
    } else {
      c.echoEl.textContent = "now " + (+v).toFixed(2);
    }
  }
}

function appendData(cols) {
  if (!plotsReady) return;
  const ref = fields.find(f => cols[f] && cols[f].length);
  if (!ref) return;
  const n = cols[ref].length;

  for (const f of fields) {
    const incoming = cols[f] || new Array(n).fill(null);
    const buf = buffers[f];
    for (let k = 0; k < incoming.length; k++) buf.push(incoming[k]);
    if (buf.length > WINDOW) buf.splice(0, buf.length - WINDOW);
  }
  if (!xField) {
    for (let k = 0; k < n; k++) sampleIdx.push((sampleIdx[sampleIdx.length - 1] ?? -1) + 1);
    if (sampleIdx.length > WINDOW) sampleIdx.splice(0, sampleIdx.length - WINDOW);
  }
  redraw();
  updateControls();
}

function redraw() {
  if (!plotsReady) return;
  const x = xField ? buffers[xField] : sampleIdx;
  for (const pl of plots) {
    pl.u.setData([x].concat(pl.fields.map(f => buffers[f])));
  }
}

function resize() {
  if (!plotsReady) return;
  for (const pl of plots) {
    const w = pl.u.root.parentElement.clientWidth || 900;
    pl.u.setSize({ width: w, height: 190 });
  }
}
window.addEventListener("resize", resize);

function connect() {
  sock = new WebSocket(`ws://${location.host}/ws`);
  sock.onopen = () => setStatus(true);
  sock.onmessage = (ev) => {
    const msg = JSON.parse(ev.data);
    if (msg.type === "init") setup(msg);
    else if (msg.type === "data") appendData(msg.cols);
  };
  sock.onclose = () => { setStatus(false); setTimeout(connect, 1000); };
  sock.onerror = () => sock.close();
}

connect();
