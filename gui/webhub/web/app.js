// openWearable web scope — vanilla + uPlot. Renders whatever dashboard the hub sends.
"use strict";

const panelsEl = document.getElementById("panels");
const statusEl = document.getElementById("status");
const titleEl = document.getElementById("title");

const WINDOW = 2000; // samples kept per field (uPlot downsamples to screen width)
const PALETTE = ["#3b82f6", "#ef4444", "#10b981", "#f59e0b",
                 "#8b5cf6", "#ec4899", "#14b8a6", "#eab308"];

let fields = [];          // schema field names, in order
let xField = null;        // x-axis field, or null -> synthetic sample index
let buffers = {};         // field -> number[]
let sampleIdx = [];       // synthetic x when xField is null
let plots = [];           // [{u, fields}]
let plotsReady = false;

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
  redraw();
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
  const ws = new WebSocket(`ws://${location.host}/ws`);
  ws.onopen = () => setStatus(true);
  ws.onmessage = (ev) => {
    const msg = JSON.parse(ev.data);
    if (msg.type === "init") setup(msg);
    else if (msg.type === "data") appendData(msg.cols);
  };
  ws.onclose = () => { setStatus(false); setTimeout(connect, 1000); };
  ws.onerror = () => ws.close();
}

connect();
