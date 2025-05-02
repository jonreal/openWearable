<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.6.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="24" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="burakkbaltaa">
<packages>
<package name="PBS0032A_N">
<smd name="1" x="-3.2" y="1.750003125" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="2" x="-3.2" y="1.25" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="3" x="-3.2" y="0.75" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="4" x="-3.2" y="0.250003125" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="5" x="-3.2" y="-0.25" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="6" x="-3.2" y="-0.75" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="7" x="-3.2" y="-1.25" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="8" x="-3.2" y="-1.75" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="9" x="-1.750003125" y="-3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="10" x="-1.25" y="-3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="11" x="-0.75" y="-3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="12" x="-0.250003125" y="-3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="13" x="0.25" y="-3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="14" x="0.75" y="-3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="15" x="1.25" y="-3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="16" x="1.75" y="-3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="17" x="3.2" y="-1.75" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="18" x="3.2" y="-1.25" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="19" x="3.2" y="-0.75" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="20" x="3.2" y="-0.25" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="21" x="3.2" y="0.250003125" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="22" x="3.2" y="0.75" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="23" x="3.2" y="1.25" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="24" x="3.2" y="1.750003125" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="25" x="1.75" y="3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="26" x="1.25" y="3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="27" x="0.75" y="3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="28" x="0.25" y="3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="29" x="-0.250003125" y="3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="30" x="-0.75" y="3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="31" x="-1.25" y="3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="32" x="-1.750003125" y="3.2" dx="1.4" dy="0.3" layer="1" roundness="100" rot="R90"/>
<wire x1="2.3368" y1="2.54" x2="2.54" y2="2.54" width="0.2032" layer="21"/>
<wire x1="2.54" y1="2.3368" x2="2.54" y2="2.54" width="0.2032" layer="21"/>
<wire x1="2.54" y1="-2.54" x2="2.54" y2="-2.3368" width="0.2032" layer="21"/>
<wire x1="2.3368" y1="-2.54" x2="2.54" y2="-2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="-2.54" x2="-2.54" y2="-2.3368" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="-2.54" x2="-2.3368" y2="-2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="2.159" x2="-2.54" y2="2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="2.54" x2="-2.3368" y2="2.54" width="0.2032" layer="21"/>
<wire x1="-3.6068" y1="2.159" x2="-2.54" y2="2.159" width="0.2032" layer="21"/>
<wire x1="-1.9558" y1="2.4892" x2="-1.8542" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="2.4892" x2="-1.651" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="2.4892" x2="-1.3462" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="2.4892" x2="-1.143" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="2.4892" x2="-0.8382" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="2.4892" x2="-0.6604" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="2.4892" x2="-0.3556" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="2.4892" x2="-0.1524" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="2.4892" x2="0.1524" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="2.4892" x2="0.3556" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="2.4892" x2="0.6604" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="2.4892" x2="0.8382" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="2.4892" x2="1.143" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="2.4892" x2="1.3462" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="2.4892" x2="1.651" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="2.4892" x2="1.8542" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="2.4892" x2="1.9558" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.9558" y1="-2.4892" x2="-1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-2.4892" x2="-1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="-2.4892" x2="-1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-2.4892" x2="-1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="-2.4892" x2="-0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-2.4892" x2="-0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="-2.4892" x2="-0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-2.4892" x2="-0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="-2.4892" x2="0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-2.4892" x2="0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-2.4892" x2="0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-2.4892" x2="0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="-2.4892" x2="1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-2.4892" x2="1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-1.8542" x2="3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="0.1524" x2="3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="2.032" y1="2.4638" x2="2.4638" y2="2.032" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.9558" x2="2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.651" x2="2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.143" x2="2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.6604" x2="2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.1524" x2="2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.3556" x2="2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.8382" x2="2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.3462" x2="2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.8542" x2="2.4892" y2="1.9558" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.8542" x2="2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.1524" x2="2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4638" y1="2.032" x2="-2.032" y2="2.4638" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.9558" x2="-2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.8542" x2="-2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.651" x2="-2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.3462" x2="-2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.143" x2="-2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.8382" x2="-2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.6604" x2="-2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.3556" x2="-2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.1524" x2="-2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.1524" x2="-2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.3556" x2="-2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.6604" x2="-2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.8382" x2="-2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.143" x2="-2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.3462" x2="-2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.651" x2="-2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.8542" x2="-2.4892" y2="1.9558" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.1524" x2="-3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.8542" x2="-3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.3556" x2="3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="0.6604" x2="3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.8382" x2="3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.6604" x2="3.5052" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="1.143" x2="3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.3462" x2="3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.143" x2="3.5052" y2="1.143" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="1.651" x2="3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.8542" x2="3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.651" x2="3.5052" y2="1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.651" x2="3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-1.3462" x2="3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.143" x2="3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.3462" x2="3.5052" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-0.8382" x2="3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.6604" x2="3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.8382" x2="3.5052" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-0.3556" x2="3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.1524" x2="3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.3556" x2="3.5052" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.1524" x2="3.5052" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.8542" x2="3.5052" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="1.651" y1="3.5052" x2="1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.651" y1="2.4892" x2="1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="2.4892" x2="1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.143" y1="3.5052" x2="1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.143" y1="2.4892" x2="1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="2.4892" x2="1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="3.5052" x2="0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="2.4892" x2="0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="2.4892" x2="0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.6604" x2="2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.143" x2="2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.651" x2="2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.3462" x2="2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.8382" x2="2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.3556" x2="2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.032" y1="-2.4638" x2="2.4638" y2="-2.032" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-3.5052" x2="0.8382" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="-3.5052" x2="0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-3.5052" x2="0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-3.5052" x2="1.3462" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="-3.5052" x2="1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-3.5052" x2="1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="-2.4892" x2="1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-2.4892" x2="1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="-2.4892" x2="1.9558" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-3.5052" x2="1.8542" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="-3.5052" x2="1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-3.5052" x2="1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="3.5052" x2="0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="2.4892" x2="0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="2.4892" x2="0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="3.5052" x2="-0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="2.4892" x2="-0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="2.4892" x2="-0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="3.5052" x2="-0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="2.4892" x2="-0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="2.4892" x2="-0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="3.5052" x2="-1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="2.4892" x2="-1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="2.4892" x2="-1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-3.5052" x2="-1.143" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="-3.5052" x2="-1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-3.5052" x2="-1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-3.5052" x2="-0.6604" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="-3.5052" x2="-0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-3.5052" x2="-0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-3.5052" x2="-0.1524" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="-3.5052" x2="-0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-3.5052" x2="-0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-3.5052" x2="0.3556" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-3.5052" x2="0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-3.5052" x2="0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="3.5052" x2="-1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="2.4892" x2="-1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="2.4892" x2="-1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.651" x2="-3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.651" x2="-2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.8542" x2="-2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.143" x2="-3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.143" x2="-2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.3462" x2="-2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.6604" x2="-3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.6604" x2="-2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.8382" x2="-2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.3556" x2="-2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.1524" x2="-2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.3556" x2="-3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.3556" x2="-2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.1524" x2="-2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.8382" x2="-3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.8382" x2="-2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.6604" x2="-2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.3462" x2="-3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.3462" x2="-2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.143" x2="-2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.651" x2="-2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="-2.4638" y1="-2.032" x2="-2.032" y2="-2.4638" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.8542" x2="-2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-3.5052" x2="-1.651" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="-3.5052" x2="-1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-3.5052" x2="-1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.9906" y1="1.4986" x2="-1.8034" y2="1.4986" width="0.1524" layer="51" curve="-180"/>
<wire x1="-1.8034" y1="1.4986" x2="-0.9906" y2="1.4986" width="0.1524" layer="51" curve="-180"/>
<wire x1="1.9558" y1="2.4892" x2="2.032" y2="2.4638" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.4638" y1="2.032" x2="2.4892" y2="1.9558" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.032" y1="-2.4638" x2="1.9558" y2="-2.4892" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.4892" y1="-1.9558" x2="2.4638" y2="-2.032" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.032" y1="2.4638" x2="-1.9558" y2="2.4892" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.4892" y1="1.9558" x2="-2.4638" y2="2.032" width="0.1524" layer="51" curve="-44"/>
<wire x1="-1.9558" y1="-2.4892" x2="-2.032" y2="-2.4638" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.4638" y1="-2.032" x2="-2.4892" y2="-1.9558" width="0.1524" layer="51" curve="-44"/>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
</package>
<package name="PBS0032A_M">
<smd name="1" x="-3.3" y="1.750003125" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="2" x="-3.3" y="1.25" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="3" x="-3.3" y="0.75" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="4" x="-3.3" y="0.250003125" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="5" x="-3.3" y="-0.25" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="6" x="-3.3" y="-0.75" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="7" x="-3.3" y="-1.25" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="8" x="-3.3" y="-1.75" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="9" x="-1.750003125" y="-3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="10" x="-1.25" y="-3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="11" x="-0.75" y="-3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="12" x="-0.250003125" y="-3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="13" x="0.25" y="-3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="14" x="0.75" y="-3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="15" x="1.25" y="-3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="16" x="1.75" y="-3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="17" x="3.3" y="-1.75" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="18" x="3.3" y="-1.25" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="19" x="3.3" y="-0.75" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="20" x="3.3" y="-0.25" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="21" x="3.3" y="0.250003125" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="22" x="3.3" y="0.75" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="23" x="3.3" y="1.25" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="24" x="3.3" y="1.750003125" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="25" x="1.75" y="3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="26" x="1.25" y="3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="27" x="0.75" y="3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="28" x="0.25" y="3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="29" x="-0.250003125" y="3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="30" x="-0.75" y="3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="31" x="-1.25" y="3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="32" x="-1.750003125" y="3.3" dx="1.6" dy="0.3" layer="1" roundness="100" rot="R90"/>
<wire x1="-3.6068" y1="2.159" x2="-2.54" y2="2.159" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="2.54" x2="-2.3368" y2="2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="2.159" x2="-2.54" y2="2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="-2.54" x2="-2.3368" y2="-2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="-2.54" x2="-2.54" y2="-2.3368" width="0.2032" layer="21"/>
<wire x1="2.3368" y1="-2.54" x2="2.54" y2="-2.54" width="0.2032" layer="21"/>
<wire x1="2.54" y1="-2.54" x2="2.54" y2="-2.3368" width="0.2032" layer="21"/>
<wire x1="2.54" y1="2.3368" x2="2.54" y2="2.54" width="0.2032" layer="21"/>
<wire x1="2.3368" y1="2.54" x2="2.54" y2="2.54" width="0.2032" layer="21"/>
<text x="-4.3688" y="5.2578" size="1.27" layer="21" ratio="6" rot="SR0">Designator763</text>
<wire x1="-1.9558" y1="2.4892" x2="-1.8542" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="2.4892" x2="-1.651" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="2.4892" x2="-1.3462" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="2.4892" x2="-1.143" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="2.4892" x2="-0.8382" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="2.4892" x2="-0.6604" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="2.4892" x2="-0.3556" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="2.4892" x2="-0.1524" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="2.4892" x2="0.1524" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="2.4892" x2="0.3556" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="2.4892" x2="0.6604" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="2.4892" x2="0.8382" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="2.4892" x2="1.143" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="2.4892" x2="1.3462" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="2.4892" x2="1.651" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="2.4892" x2="1.8542" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="2.4892" x2="1.9558" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.9558" y1="-2.4892" x2="-1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-2.4892" x2="-1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="-2.4892" x2="-1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-2.4892" x2="-1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="-2.4892" x2="-0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-2.4892" x2="-0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="-2.4892" x2="-0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-2.4892" x2="-0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="-2.4892" x2="0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-2.4892" x2="0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-2.4892" x2="0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-2.4892" x2="0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="-2.4892" x2="1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-2.4892" x2="1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-1.8542" x2="3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="0.1524" x2="3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="2.032" y1="2.4638" x2="2.4638" y2="2.032" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.9558" x2="2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.651" x2="2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.143" x2="2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.6604" x2="2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.1524" x2="2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.3556" x2="2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.8382" x2="2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.3462" x2="2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.8542" x2="2.4892" y2="1.9558" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.8542" x2="2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.1524" x2="2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4638" y1="2.032" x2="-2.032" y2="2.4638" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.9558" x2="-2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.8542" x2="-2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.651" x2="-2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.3462" x2="-2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.143" x2="-2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.8382" x2="-2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.6604" x2="-2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.3556" x2="-2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.1524" x2="-2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.1524" x2="-2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.3556" x2="-2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.6604" x2="-2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.8382" x2="-2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.143" x2="-2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.3462" x2="-2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.651" x2="-2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.8542" x2="-2.4892" y2="1.9558" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.1524" x2="-3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.8542" x2="-3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.3556" x2="3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="0.6604" x2="3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.8382" x2="3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.6604" x2="3.5052" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="1.143" x2="3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.3462" x2="3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.143" x2="3.5052" y2="1.143" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="1.651" x2="3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.8542" x2="3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.651" x2="3.5052" y2="1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.651" x2="3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-1.3462" x2="3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.143" x2="3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.3462" x2="3.5052" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-0.8382" x2="3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.6604" x2="3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.8382" x2="3.5052" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-0.3556" x2="3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.1524" x2="3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.3556" x2="3.5052" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.1524" x2="3.5052" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.8542" x2="3.5052" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="1.651" y1="3.5052" x2="1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.651" y1="2.4892" x2="1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="2.4892" x2="1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.143" y1="3.5052" x2="1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.143" y1="2.4892" x2="1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="2.4892" x2="1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="3.5052" x2="0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="2.4892" x2="0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="2.4892" x2="0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.6604" x2="2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.143" x2="2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.651" x2="2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.3462" x2="2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.8382" x2="2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.3556" x2="2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.032" y1="-2.4638" x2="2.4638" y2="-2.032" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-3.5052" x2="0.8382" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="-3.5052" x2="0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-3.5052" x2="0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-3.5052" x2="1.3462" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="-3.5052" x2="1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-3.5052" x2="1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="-2.4892" x2="1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-2.4892" x2="1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="-2.4892" x2="1.9558" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-3.5052" x2="1.8542" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="-3.5052" x2="1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-3.5052" x2="1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="3.5052" x2="0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="2.4892" x2="0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="2.4892" x2="0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="3.5052" x2="-0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="2.4892" x2="-0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="2.4892" x2="-0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="3.5052" x2="-0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="2.4892" x2="-0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="2.4892" x2="-0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="3.5052" x2="-1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="2.4892" x2="-1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="2.4892" x2="-1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-3.5052" x2="-1.143" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="-3.5052" x2="-1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-3.5052" x2="-1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-3.5052" x2="-0.6604" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="-3.5052" x2="-0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-3.5052" x2="-0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-3.5052" x2="-0.1524" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="-3.5052" x2="-0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-3.5052" x2="-0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-3.5052" x2="0.3556" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-3.5052" x2="0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-3.5052" x2="0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="3.5052" x2="-1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="2.4892" x2="-1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="2.4892" x2="-1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.651" x2="-3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.651" x2="-2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.8542" x2="-2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.143" x2="-3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.143" x2="-2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.3462" x2="-2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.6604" x2="-3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.6604" x2="-2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.8382" x2="-2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.3556" x2="-2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.1524" x2="-2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.3556" x2="-3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.3556" x2="-2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.1524" x2="-2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.8382" x2="-3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.8382" x2="-2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.6604" x2="-2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.3462" x2="-3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.3462" x2="-2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.143" x2="-2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.651" x2="-2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="-2.4638" y1="-2.032" x2="-2.032" y2="-2.4638" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.8542" x2="-2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-3.5052" x2="-1.651" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="-3.5052" x2="-1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-3.5052" x2="-1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.9906" y1="1.4986" x2="-1.8034" y2="1.4986" width="0.1524" layer="51" curve="-180"/>
<wire x1="-1.8034" y1="1.4986" x2="-0.9906" y2="1.4986" width="0.1524" layer="51" curve="-180"/>
<wire x1="1.9558" y1="2.4892" x2="2.032" y2="2.4638" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.4638" y1="2.032" x2="2.4892" y2="1.9558" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.032" y1="-2.4638" x2="1.9558" y2="-2.4892" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.4892" y1="-1.9558" x2="2.4638" y2="-2.032" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.032" y1="2.4638" x2="-1.9558" y2="2.4892" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.4892" y1="1.9558" x2="-2.4638" y2="2.032" width="0.1524" layer="51" curve="-44"/>
<wire x1="-1.9558" y1="-2.4892" x2="-2.032" y2="-2.4638" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.4638" y1="-2.032" x2="-2.4892" y2="-1.9558" width="0.1524" layer="51" curve="-44"/>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
</package>
<package name="PBS0032A_L">
<smd name="1" x="-3.1" y="1.75" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="2" x="-3.1" y="1.25" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="3" x="-3.1" y="0.75" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="4" x="-3.1" y="0.25" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="5" x="-3.1" y="-0.250003125" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="6" x="-3.1" y="-0.75" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="7" x="-3.1" y="-1.25" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="8" x="-3.1" y="-1.750003125" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="9" x="-1.75" y="-3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="10" x="-1.25" y="-3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="11" x="-0.75" y="-3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="12" x="-0.25" y="-3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="13" x="0.250003125" y="-3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="14" x="0.75" y="-3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="15" x="1.25" y="-3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="16" x="1.750003125" y="-3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="17" x="3.1" y="-1.750003125" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="18" x="3.1" y="-1.25" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="19" x="3.1" y="-0.75" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="20" x="3.1" y="-0.250003125" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="21" x="3.1" y="0.25" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="22" x="3.1" y="0.75" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="23" x="3.1" y="1.25" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="24" x="3.1" y="1.75" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R180"/>
<smd name="25" x="1.750003125" y="3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="26" x="1.25" y="3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="27" x="0.75" y="3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="28" x="0.250003125" y="3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="29" x="-0.25" y="3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="30" x="-0.75" y="3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="31" x="-1.25" y="3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<smd name="32" x="-1.75" y="3.1" dx="1.2" dy="0.3" layer="1" roundness="100" rot="R90"/>
<wire x1="2.3368" y1="2.54" x2="2.54" y2="2.54" width="0.2032" layer="21"/>
<wire x1="2.54" y1="2.3368" x2="2.54" y2="2.54" width="0.2032" layer="21"/>
<wire x1="2.54" y1="-2.54" x2="2.54" y2="-2.3368" width="0.2032" layer="21"/>
<wire x1="2.3368" y1="-2.54" x2="2.54" y2="-2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="-2.54" x2="-2.54" y2="-2.3368" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="-2.54" x2="-2.3368" y2="-2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="2.159" x2="-2.54" y2="2.54" width="0.2032" layer="21"/>
<wire x1="-2.54" y1="2.54" x2="-2.3368" y2="2.54" width="0.2032" layer="21"/>
<wire x1="-3.6068" y1="2.159" x2="-2.54" y2="2.159" width="0.2032" layer="21"/>
<text x="-3.5814" y="4.4704" size="1.27" layer="21" ratio="6" rot="SR0">Designator762</text>
<wire x1="-1.9558" y1="2.4892" x2="-1.8542" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="2.4892" x2="-1.651" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="2.4892" x2="-1.3462" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="2.4892" x2="-1.143" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="2.4892" x2="-0.8382" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="2.4892" x2="-0.6604" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="2.4892" x2="-0.3556" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="2.4892" x2="-0.1524" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="2.4892" x2="0.1524" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="2.4892" x2="0.3556" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="2.4892" x2="0.6604" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="2.4892" x2="0.8382" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="2.4892" x2="1.143" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="2.4892" x2="1.3462" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="2.4892" x2="1.651" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="2.4892" x2="1.8542" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="2.4892" x2="1.9558" y2="2.4892" width="0.1524" layer="51"/>
<wire x1="-1.9558" y1="-2.4892" x2="-1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-2.4892" x2="-1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="-2.4892" x2="-1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-2.4892" x2="-1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="-2.4892" x2="-0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-2.4892" x2="-0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="-2.4892" x2="-0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-2.4892" x2="-0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="-2.4892" x2="0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-2.4892" x2="0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-2.4892" x2="0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-2.4892" x2="0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="-2.4892" x2="1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-2.4892" x2="1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-1.8542" x2="3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="0.1524" x2="3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="2.032" y1="2.4638" x2="2.4638" y2="2.032" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.9558" x2="2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.651" x2="2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.143" x2="2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.6604" x2="2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.1524" x2="2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.3556" x2="2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.8382" x2="2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.3462" x2="2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.8542" x2="2.4892" y2="1.9558" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.8542" x2="2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.1524" x2="2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4638" y1="2.032" x2="-2.032" y2="2.4638" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.9558" x2="-2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.8542" x2="-2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.651" x2="-2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.3462" x2="-2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-1.143" x2="-2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.8382" x2="-2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.6604" x2="-2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.3556" x2="-2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="-0.1524" x2="-2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.1524" x2="-2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.3556" x2="-2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.6604" x2="-2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="0.8382" x2="-2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.143" x2="-2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.3462" x2="-2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.651" x2="-2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-2.4892" y1="1.8542" x2="-2.4892" y2="1.9558" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.1524" x2="-3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.8542" x2="-3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.3556" x2="3.5052" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="0.6604" x2="3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.8382" x2="3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.6604" x2="3.5052" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="1.143" x2="3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.3462" x2="3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.143" x2="3.5052" y2="1.143" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="1.651" x2="3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.8542" x2="3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.651" x2="3.5052" y2="1.651" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.651" x2="3.5052" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-1.3462" x2="3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.143" x2="3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.3462" x2="3.5052" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-0.8382" x2="3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.6604" x2="3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.8382" x2="3.5052" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="3.5052" y1="-0.3556" x2="3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.1524" x2="3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.3556" x2="3.5052" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.1524" x2="3.5052" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.8542" x2="3.5052" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="1.651" y1="3.5052" x2="1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.651" y1="2.4892" x2="1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="2.4892" x2="1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.143" y1="3.5052" x2="1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.143" y1="2.4892" x2="1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="2.4892" x2="1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="3.5052" x2="0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="2.4892" x2="0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="2.4892" x2="0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="0.6604" x2="2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.143" x2="2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="1.651" x2="2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-1.3462" x2="2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.8382" x2="2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="2.4892" y1="-0.3556" x2="2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="2.032" y1="-2.4638" x2="2.4638" y2="-2.032" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-3.5052" x2="0.8382" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="0.8382" y1="-3.5052" x2="0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.6604" y1="-3.5052" x2="0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-3.5052" x2="1.3462" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="-3.5052" x2="1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.143" y1="-3.5052" x2="1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.3462" y1="-2.4892" x2="1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-2.4892" x2="1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="-2.4892" x2="1.9558" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-3.5052" x2="1.8542" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="1.8542" y1="-3.5052" x2="1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="1.651" y1="-3.5052" x2="1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="3.5052" x2="0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="2.4892" x2="0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="2.4892" x2="0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="3.5052" x2="-0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="2.4892" x2="-0.3556" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="2.4892" x2="-0.1524" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="3.5052" x2="-0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="2.4892" x2="-0.8382" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="2.4892" x2="-0.6604" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="3.5052" x2="-1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="2.4892" x2="-1.3462" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="2.4892" x2="-1.143" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-3.5052" x2="-1.143" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-1.143" y1="-3.5052" x2="-1.143" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.3462" y1="-3.5052" x2="-1.3462" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-3.5052" x2="-0.6604" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-0.6604" y1="-3.5052" x2="-0.6604" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.8382" y1="-3.5052" x2="-0.8382" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-3.5052" x2="-0.1524" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-0.1524" y1="-3.5052" x2="-0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-3.5052" x2="-0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-3.5052" x2="0.3556" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-3.5052" x2="0.3556" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="0.1524" y1="-3.5052" x2="0.1524" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="3.5052" x2="-1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="2.4892" x2="-1.8542" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="2.4892" x2="-1.651" y2="3.5052" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.651" x2="-3.5052" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.651" x2="-2.4892" y2="1.651" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.8542" x2="-2.4892" y2="1.8542" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.143" x2="-3.5052" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.143" x2="-2.4892" y2="1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="1.3462" x2="-2.4892" y2="1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.6604" x2="-3.5052" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.6604" x2="-2.4892" y2="0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.8382" x2="-2.4892" y2="0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.3556" x2="-2.4892" y2="0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="0.1524" x2="-2.4892" y2="0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.3556" x2="-3.5052" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.3556" x2="-2.4892" y2="-0.3556" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.1524" x2="-2.4892" y2="-0.1524" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.8382" x2="-3.5052" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.8382" x2="-2.4892" y2="-0.8382" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-0.6604" x2="-2.4892" y2="-0.6604" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.3462" x2="-3.5052" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.3462" x2="-2.4892" y2="-1.3462" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.143" x2="-2.4892" y2="-1.143" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.651" x2="-2.4892" y2="-1.651" width="0.1524" layer="51"/>
<wire x1="-2.4638" y1="-2.032" x2="-2.032" y2="-2.4638" width="0.1524" layer="51"/>
<wire x1="-3.5052" y1="-1.8542" x2="-2.4892" y2="-1.8542" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-3.5052" x2="-1.651" y2="-3.5052" width="0.1524" layer="51"/>
<wire x1="-1.651" y1="-3.5052" x2="-1.651" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-1.8542" y1="-3.5052" x2="-1.8542" y2="-2.4892" width="0.1524" layer="51"/>
<wire x1="-0.9906" y1="1.4986" x2="-1.8034" y2="1.4986" width="0.1524" layer="51" curve="-180"/>
<wire x1="-1.8034" y1="1.4986" x2="-0.9906" y2="1.4986" width="0.1524" layer="51" curve="-180"/>
<wire x1="1.9558" y1="2.4892" x2="2.032" y2="2.4638" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.4638" y1="2.032" x2="2.4892" y2="1.9558" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.032" y1="-2.4638" x2="1.9558" y2="-2.4892" width="0.1524" layer="51" curve="-44"/>
<wire x1="2.4892" y1="-1.9558" x2="2.4638" y2="-2.032" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.032" y1="2.4638" x2="-1.9558" y2="2.4892" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.4892" y1="1.9558" x2="-2.4638" y2="2.032" width="0.1524" layer="51" curve="-44"/>
<wire x1="-1.9558" y1="-2.4892" x2="-2.032" y2="-2.4638" width="0.1524" layer="51" curve="-44"/>
<wire x1="-2.4638" y1="-2.032" x2="-2.4892" y2="-1.9558" width="0.1524" layer="51" curve="-44"/>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
</package>
<package name="CONN_282834-8_TEC">
<pad name="1" x="0" y="0" drill="1.1938" diameter="1.7018"/>
<pad name="2" x="2.54" y="0" drill="1.1938" diameter="1.7018"/>
<pad name="3" x="5.08" y="0" drill="1.1938" diameter="1.7018"/>
<pad name="4" x="7.62" y="0" drill="1.1938" diameter="1.7018"/>
<pad name="5" x="10.16" y="0" drill="1.1938" diameter="1.7018"/>
<pad name="6" x="12.7" y="0" drill="1.1938" diameter="1.7018"/>
<pad name="7" x="15.24" y="0" drill="1.1938" diameter="1.7018"/>
<pad name="8" x="17.78" y="0" drill="1.1938" diameter="1.7018"/>
<wire x1="0" y1="0" x2="0" y2="5.4102" width="0.1524" layer="48"/>
<wire x1="0" y1="5.4102" x2="0" y2="5.7912" width="0.1524" layer="48"/>
<wire x1="2.54" y1="0" x2="2.54" y2="5.4102" width="0.1524" layer="48"/>
<wire x1="2.54" y1="5.4102" x2="2.54" y2="5.7912" width="0.1524" layer="48"/>
<wire x1="0" y1="5.4102" x2="-1.27" y2="5.4102" width="0.1524" layer="48"/>
<wire x1="2.54" y1="5.4102" x2="3.81" y2="5.4102" width="0.1524" layer="48"/>
<wire x1="0" y1="5.4102" x2="-0.254" y2="5.5372" width="0.1524" layer="48"/>
<wire x1="0" y1="5.4102" x2="-0.254" y2="5.2832" width="0.1524" layer="48"/>
<wire x1="-0.254" y1="5.5372" x2="-0.254" y2="5.2832" width="0.1524" layer="48"/>
<wire x1="2.54" y1="5.4102" x2="2.794" y2="5.5372" width="0.1524" layer="48"/>
<wire x1="2.54" y1="5.4102" x2="2.794" y2="5.2832" width="0.1524" layer="48"/>
<wire x1="2.794" y1="5.5372" x2="2.794" y2="5.2832" width="0.1524" layer="48"/>
<wire x1="-1.4986" y1="3.2512" x2="-1.4986" y2="13.6652" width="0.1524" layer="48"/>
<wire x1="-1.4986" y1="13.6652" x2="-1.4986" y2="14.0462" width="0.1524" layer="48"/>
<wire x1="19.2786" y1="3.2512" x2="19.2786" y2="13.6652" width="0.1524" layer="48"/>
<wire x1="19.2786" y1="13.6652" x2="19.2786" y2="14.0462" width="0.1524" layer="48"/>
<wire x1="-1.4986" y1="13.6652" x2="19.2786" y2="13.6652" width="0.1524" layer="48"/>
<wire x1="-1.4986" y1="13.6652" x2="-1.2446" y2="13.7922" width="0.1524" layer="48"/>
<wire x1="-1.4986" y1="13.6652" x2="-1.2446" y2="13.5382" width="0.1524" layer="48"/>
<wire x1="-1.2446" y1="13.7922" x2="-1.2446" y2="13.5382" width="0.1524" layer="48"/>
<wire x1="19.2786" y1="13.6652" x2="19.0246" y2="13.7922" width="0.1524" layer="48"/>
<wire x1="19.2786" y1="13.6652" x2="19.0246" y2="13.5382" width="0.1524" layer="48"/>
<wire x1="19.0246" y1="13.7922" x2="19.0246" y2="13.5382" width="0.1524" layer="48"/>
<wire x1="0" y1="0" x2="21.8186" y2="0" width="0.1524" layer="48"/>
<wire x1="21.8186" y1="0" x2="22.1996" y2="0" width="0.1524" layer="48"/>
<wire x1="0" y1="-25.4" x2="21.8186" y2="-25.4" width="0.1524" layer="48"/>
<wire x1="21.8186" y1="-25.4" x2="22.1996" y2="-25.4" width="0.1524" layer="48"/>
<wire x1="21.8186" y1="0" x2="21.8186" y2="-25.4" width="0.1524" layer="48"/>
<wire x1="21.8186" y1="0" x2="21.6916" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="21.8186" y1="0" x2="21.9456" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="21.6916" y1="-0.254" x2="21.9456" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="21.8186" y1="-25.4" x2="21.6916" y2="-25.146" width="0.1524" layer="48"/>
<wire x1="21.8186" y1="-25.4" x2="21.9456" y2="-25.146" width="0.1524" layer="48"/>
<wire x1="21.6916" y1="-25.146" x2="21.9456" y2="-25.146" width="0.1524" layer="48"/>
<wire x1="-11.9126" y1="3.2512" x2="-12.2936" y2="3.2512" width="0.1524" layer="48"/>
<wire x1="0" y1="0" x2="-11.9126" y2="0" width="0.1524" layer="48"/>
<wire x1="-11.9126" y1="0" x2="-12.2936" y2="0" width="0.1524" layer="48"/>
<wire x1="-11.9126" y1="3.2512" x2="-11.9126" y2="4.5212" width="0.1524" layer="48"/>
<wire x1="-11.9126" y1="0" x2="-11.9126" y2="-1.27" width="0.1524" layer="48"/>
<wire x1="-11.9126" y1="3.2512" x2="-12.0396" y2="3.5052" width="0.1524" layer="48"/>
<wire x1="-11.9126" y1="3.2512" x2="-11.7856" y2="3.5052" width="0.1524" layer="48"/>
<wire x1="-12.0396" y1="3.5052" x2="-11.7856" y2="3.5052" width="0.1524" layer="48"/>
<wire x1="-11.9126" y1="0" x2="-12.0396" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="-11.9126" y1="0" x2="-11.7856" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="-12.0396" y1="-0.254" x2="-11.7856" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="-1.4986" y1="3.2512" x2="-10.6426" y2="3.2512" width="0.1524" layer="48"/>
<wire x1="-10.6426" y1="3.2512" x2="-11.9126" y2="3.2512" width="0.1524" layer="48"/>
<wire x1="-1.4986" y1="-3.2512" x2="-10.6426" y2="-3.2512" width="0.1524" layer="48"/>
<wire x1="-10.6426" y1="-3.2512" x2="-11.0236" y2="-3.2512" width="0.1524" layer="48"/>
<wire x1="-10.6426" y1="3.2512" x2="-10.6426" y2="-3.2512" width="0.1524" layer="48"/>
<wire x1="-10.6426" y1="3.2512" x2="-10.7696" y2="2.9972" width="0.1524" layer="48"/>
<wire x1="-10.6426" y1="3.2512" x2="-10.5156" y2="2.9972" width="0.1524" layer="48"/>
<wire x1="-10.7696" y1="2.9972" x2="-10.5156" y2="2.9972" width="0.1524" layer="48"/>
<wire x1="-10.6426" y1="-3.2512" x2="-10.7696" y2="-2.9972" width="0.1524" layer="48"/>
<wire x1="-10.6426" y1="-3.2512" x2="-10.5156" y2="-2.9972" width="0.1524" layer="48"/>
<wire x1="-10.7696" y1="-2.9972" x2="-10.5156" y2="-2.9972" width="0.1524" layer="48"/>
<text x="-6.8834" y="-7.3152" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: EX67Y67D47P</text>
<text x="-7.6454" y="-9.2202" size="1.27" layer="48" ratio="6" rot="SR0">1st Mtg Padstyle: RX110Y80D50P</text>
<text x="-7.2644" y="-11.1252" size="1.27" layer="48" ratio="6" rot="SR0">2nd Mtg Padstyle: EX70Y70D70P</text>
<text x="-7.2644" y="-13.0302" size="1.27" layer="48" ratio="6" rot="SR0">3rd Mtg Padstyle: EX70Y70D70P</text>
<text x="-7.4676" y="-14.9352" size="1.27" layer="48" ratio="6" rot="SR0">Left Mtg Padstyle: EX60Y60D30P</text>
<text x="-8.0518" y="-16.8402" size="1.27" layer="48" ratio="6" rot="SR0">Right Mtg Padstyle: EX60Y60D30P</text>
<text x="-5.9182" y="-18.7452" size="1.27" layer="48" ratio="6" rot="SR0">Alt Padstyle 1: OX60Y90D30P</text>
<text x="-5.9182" y="-20.6502" size="1.27" layer="48" ratio="6" rot="SR0">Alt Padstyle 2: OX90Y60D30P</text>
<text x="0" y="5.9182" size="0.635" layer="48" ratio="4" rot="SR0">0.1in/2.54mm</text>
<text x="4.572" y="14.1732" size="0.635" layer="48" ratio="4" rot="SR0">0.818in/20.777mm</text>
<text x="22.3266" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">1in/25.4mm</text>
<text x="-19.939" y="1.2954" size="0.635" layer="48" ratio="4" rot="SR0">0.128in/3.25mm</text>
<text x="-19.2278" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.256in/6.502mm</text>
<wire x1="-1.6256" y1="-3.3782" x2="19.4056" y2="-3.3782" width="0.1524" layer="21"/>
<wire x1="19.4056" y1="-3.3782" x2="19.4056" y2="3.3782" width="0.1524" layer="21"/>
<wire x1="19.4056" y1="3.3782" x2="-1.6256" y2="3.3782" width="0.1524" layer="21"/>
<wire x1="-1.6256" y1="3.3782" x2="-1.6256" y2="-3.3782" width="0.1524" layer="21"/>
<wire x1="-3.0226" y1="0" x2="-3.7846" y2="0" width="0.508" layer="21" curve="-180"/>
<wire x1="-3.7846" y1="0" x2="-3.0226" y2="0" width="0.508" layer="21" curve="-180"/>
<wire x1="-1.4986" y1="-3.2512" x2="19.2786" y2="-3.2512" width="0.1524" layer="51"/>
<wire x1="19.2786" y1="-3.2512" x2="19.2786" y2="3.2512" width="0.1524" layer="51"/>
<wire x1="19.2786" y1="3.2512" x2="-1.4986" y2="3.2512" width="0.1524" layer="51"/>
<wire x1="-1.4986" y1="3.2512" x2="-1.4986" y2="-3.2512" width="0.1524" layer="51"/>
<wire x1="0.381" y1="1.905" x2="-0.381" y2="1.905" width="0.508" layer="51" curve="-180"/>
<wire x1="-0.381" y1="1.905" x2="0.381" y2="1.905" width="0.508" layer="51" curve="-180"/>
<wire x1="-3.0226" y1="0" x2="-3.7846" y2="0" width="0.508" layer="22" curve="-180"/>
<wire x1="-3.7846" y1="0" x2="-3.0226" y2="0" width="0.508" layer="22" curve="-180"/>
<text x="5.6134" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="7.1628" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="RC0402N_YAG-M">
<smd name="1" x="-0.6604" y="0" dx="0.8636" dy="0.5588" layer="1"/>
<smd name="2" x="0.6604" y="0" dx="0.8636" dy="0.5588" layer="1"/>
<wire x1="0" y1="0.254" x2="3.0734" y2="0.254" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.254" x2="3.0734" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="0" x2="-0.5334" y2="2.794" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="0" x2="0.2286" y2="-2.794" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.1374" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX34Y22D0T</text>
<text x="-18.0848" y="-9.6774" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.2174" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<text x="-15.2146" y="-7.1374" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX34Y22D0T</text>
<text x="-18.0848" y="-9.6774" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.2174" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="0.5334" y1="2.794" x2="0.5334" y2="-2.794" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.1374" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX34Y22D0T</text>
<text x="-18.0848" y="-9.6774" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.2174" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="3.0734" y1="0.254" x2="3.4544" y2="0.254" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.4544" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="3.0734" y2="1.524" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.0734" y2="-1.524" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="2.9464" y2="0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="3.2004" y2="0.508" width="0.1524" layer="48"/>
<wire x1="2.9464" y1="0.508" x2="3.2004" y2="0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="2.9464" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.2004" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="2.9464" y1="-0.508" x2="3.2004" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.5334" y2="3.175" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.5334" y2="3.175" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-1.8034" y2="2.794" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="1.8034" y2="2.794" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.7874" y2="2.921" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="-0.7874" y1="2.921" x2="-0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.7874" y2="2.921" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.7874" y1="2.921" x2="0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="0.2286" y2="-3.175" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.5334" y2="-3.175" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-1.0414" y2="-2.794" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="1.8034" y2="-2.794" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-0.0254" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-0.0254" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="-0.0254" y1="-2.667" x2="-0.0254" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.7874" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.7874" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="0.7874" y1="-2.667" x2="0.7874" y2="-2.921" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.1374" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX34Y22D0T</text>
<text x="-18.0848" y="-9.6774" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.2174" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<wire x1="-0.2286" y1="-0.254" x2="-0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="0.254" x2="-0.5334" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.5334" y1="-0.254" x2="-0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="0.254" x2="0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="-0.254" x2="0.5334" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.5334" y1="0.254" x2="0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="-0.254" x2="0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.5334" y1="-0.254" x2="0.5334" y2="0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="0.254" x2="-0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.5334" y1="0.254" x2="-0.5334" y2="-0.254" width="0.1524" layer="51"/>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="RC0402N_YAG">
<smd name="1" x="-0.5588" y="0" dx="0.6604" dy="0.508" layer="1"/>
<smd name="2" x="0.5588" y="0" dx="0.6604" dy="0.508" layer="1"/>
<wire x1="0" y1="0.254" x2="3.0734" y2="0.254" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.254" x2="3.0734" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="0" x2="-0.5334" y2="2.794" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="0" x2="0.2286" y2="-2.794" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.112" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX26Y20D0T</text>
<text x="-18.0848" y="-9.652" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.192" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.0254" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.0254" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<text x="-15.2146" y="-7.112" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX26Y20D0T</text>
<text x="-18.0848" y="-9.652" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.192" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.0254" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.0254" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="0.5334" y1="2.794" x2="0.5334" y2="-2.794" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.112" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX26Y20D0T</text>
<text x="-18.0848" y="-9.652" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.192" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.0254" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.0254" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="3.0734" y1="0.254" x2="3.4544" y2="0.254" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.4544" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="3.0734" y2="1.524" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.0734" y2="-1.524" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="2.9464" y2="0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="3.2004" y2="0.508" width="0.1524" layer="48"/>
<wire x1="2.9464" y1="0.508" x2="3.2004" y2="0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="2.9464" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.2004" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="2.9464" y1="-0.508" x2="3.2004" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.5334" y2="3.175" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.5334" y2="3.175" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-1.8034" y2="2.794" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="1.8034" y2="2.794" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.7874" y2="2.921" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="-0.7874" y1="2.921" x2="-0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.7874" y2="2.921" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.7874" y1="2.921" x2="0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="0.2286" y2="-3.175" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.5334" y2="-3.175" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-1.0414" y2="-2.794" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="1.8034" y2="-2.794" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-0.0254" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-0.0254" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="-0.0254" y1="-2.667" x2="-0.0254" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.7874" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.7874" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="0.7874" y1="-2.667" x2="0.7874" y2="-2.921" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.112" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX26Y20D0T</text>
<text x="-18.0848" y="-9.652" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.192" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<wire x1="-0.2286" y1="-0.254" x2="-0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="0.254" x2="-0.5334" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.5334" y1="-0.254" x2="-0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="0.254" x2="0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="-0.254" x2="0.5334" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.5334" y1="0.254" x2="0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="-0.254" x2="0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.5334" y1="-0.254" x2="0.5334" y2="0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="0.254" x2="-0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.5334" y1="0.254" x2="-0.5334" y2="-0.254" width="0.1524" layer="51"/>
<polygon width="0.0254" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.0254" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="RC0402N_YAG-L">
<smd name="1" x="-0.4572" y="0" dx="0.4572" dy="0.4572" layer="1"/>
<smd name="2" x="0.4572" y="0" dx="0.4572" dy="0.4572" layer="1"/>
<wire x1="0" y1="0.254" x2="3.0734" y2="0.254" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.254" x2="3.0734" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="0" x2="-0.5334" y2="2.794" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="0" x2="0.2286" y2="-2.794" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.0866" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX18Y18D0T</text>
<text x="-18.0848" y="-9.6266" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.1666" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<text x="-15.2146" y="-7.0866" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX18Y18D0T</text>
<text x="-18.0848" y="-9.6266" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.1666" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="0.5334" y1="2.794" x2="0.5334" y2="-2.794" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.0866" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX18Y18D0T</text>
<text x="-18.0848" y="-9.6266" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.1666" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="3.0734" y1="0.254" x2="3.4544" y2="0.254" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.4544" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="3.0734" y2="1.524" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.0734" y2="-1.524" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="2.9464" y2="0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="0.254" x2="3.2004" y2="0.508" width="0.1524" layer="48"/>
<wire x1="2.9464" y1="0.508" x2="3.2004" y2="0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="2.9464" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="3.0734" y1="-0.254" x2="3.2004" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="2.9464" y1="-0.508" x2="3.2004" y2="-0.508" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.5334" y2="3.175" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.5334" y2="3.175" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-1.8034" y2="2.794" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="1.8034" y2="2.794" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.7874" y2="2.921" width="0.1524" layer="48"/>
<wire x1="-0.5334" y1="2.794" x2="-0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="-0.7874" y1="2.921" x2="-0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.7874" y2="2.921" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="2.794" x2="0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.7874" y1="2.921" x2="0.7874" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="0.2286" y2="-3.175" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.5334" y2="-3.175" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-1.0414" y2="-2.794" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="1.8034" y2="-2.794" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-0.0254" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="0.2286" y1="-2.794" x2="-0.0254" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="-0.0254" y1="-2.667" x2="-0.0254" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.7874" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="0.5334" y1="-2.794" x2="0.7874" y2="-2.921" width="0.1524" layer="48"/>
<wire x1="0.7874" y1="-2.667" x2="0.7874" y2="-2.921" width="0.1524" layer="48"/>
<text x="-15.2146" y="-7.0866" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX18Y18D0T</text>
<text x="-18.0848" y="-9.6266" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.1666" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="3.5814" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.02in/0.508mm</text>
<text x="-4.0386" y="3.302" size="0.635" layer="48" ratio="4" rot="SR0">0.042in/1.067mm</text>
<text x="-3.6576" y="-3.937" size="0.635" layer="48" ratio="4" rot="SR0">0.012in/0.305mm</text>
<wire x1="-0.2286" y1="-0.254" x2="-0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="0.254" x2="-0.5334" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.5334" y1="-0.254" x2="-0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="0.254" x2="0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="-0.254" x2="0.5334" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.5334" y1="0.254" x2="0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="-0.254" x2="0.2286" y2="-0.254" width="0.1524" layer="51"/>
<wire x1="0.5334" y1="-0.254" x2="0.5334" y2="0.254" width="0.1524" layer="51"/>
<wire x1="0.2286" y1="0.254" x2="-0.2286" y2="0.254" width="0.1524" layer="51"/>
<wire x1="-0.5334" y1="0.254" x2="-0.5334" y2="-0.254" width="0.1524" layer="51"/>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.2032"/>
<vertex x="0.1778" y="0.2032"/>
<vertex x="0.1778" y="-0.2032"/>
<vertex x="-0.1778" y="-0.2032"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.1778" y="0.254"/>
<vertex x="0.1778" y="0.254"/>
<vertex x="0.1778" y="-0.254"/>
<vertex x="-0.1778" y="-0.254"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="LED_LSTS-C190_LTO">
<smd name="1" x="-0.75" y="0" dx="0.8128" dy="0.8128" layer="1"/>
<smd name="2" x="0.75" y="0" dx="0.8128" dy="0.8128" layer="1" rot="R180"/>
<wire x1="-0.762" y1="0" x2="-0.762" y2="2.54" width="0.1524" layer="48"/>
<wire x1="-0.762" y1="2.54" x2="-0.762" y2="2.921" width="0.1524" layer="48"/>
<wire x1="0.762" y1="0" x2="0.762" y2="2.54" width="0.1524" layer="48"/>
<wire x1="0.762" y1="2.54" x2="0.762" y2="2.921" width="0.1524" layer="48"/>
<wire x1="-0.762" y1="2.54" x2="-2.032" y2="2.54" width="0.1524" layer="48"/>
<wire x1="0.762" y1="2.54" x2="2.032" y2="2.54" width="0.1524" layer="48"/>
<wire x1="-0.762" y1="2.54" x2="-1.016" y2="2.667" width="0.1524" layer="48"/>
<wire x1="-0.762" y1="2.54" x2="-1.016" y2="2.413" width="0.1524" layer="48"/>
<wire x1="-1.016" y1="2.667" x2="-1.016" y2="2.413" width="0.1524" layer="48"/>
<wire x1="0.762" y1="2.54" x2="1.016" y2="2.667" width="0.1524" layer="48"/>
<wire x1="0.762" y1="2.54" x2="1.016" y2="2.413" width="0.1524" layer="48"/>
<wire x1="1.016" y1="2.667" x2="1.016" y2="2.413" width="0.1524" layer="48"/>
<wire x1="-0.762" y1="0" x2="-3.302" y2="0" width="0.1524" layer="48"/>
<wire x1="-3.302" y1="0" x2="-3.683" y2="0" width="0.1524" layer="48"/>
<wire x1="-3.302" y1="0" x2="-3.302" y2="1.27" width="0.1524" layer="48"/>
<wire x1="-3.302" y1="0" x2="-3.302" y2="-1.27" width="0.1524" layer="48"/>
<wire x1="-3.302" y1="0" x2="-3.429" y2="0.254" width="0.1524" layer="48"/>
<wire x1="-3.302" y1="0" x2="-3.175" y2="0.254" width="0.1524" layer="48"/>
<wire x1="-3.429" y1="0.254" x2="-3.175" y2="0.254" width="0.1524" layer="48"/>
<wire x1="-3.302" y1="0" x2="-3.429" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="-3.302" y1="0" x2="-3.175" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="-3.429" y1="-0.254" x2="-3.175" y2="-0.254" width="0.1524" layer="48"/>
<wire x1="0.8128" y1="0.4064" x2="3.302" y2="0.4064" width="0.1524" layer="48"/>
<wire x1="3.302" y1="0.4064" x2="3.683" y2="0.4064" width="0.1524" layer="48"/>
<wire x1="0.8128" y1="-0.4064" x2="3.302" y2="-0.4064" width="0.1524" layer="48"/>
<wire x1="3.302" y1="-0.4064" x2="3.683" y2="-0.4064" width="0.1524" layer="48"/>
<wire x1="3.302" y1="0.4064" x2="3.302" y2="1.6764" width="0.1524" layer="48"/>
<wire x1="3.302" y1="-0.4064" x2="3.302" y2="-1.6764" width="0.1524" layer="48"/>
<wire x1="3.302" y1="0.4064" x2="3.175" y2="0.6604" width="0.1524" layer="48"/>
<wire x1="3.302" y1="0.4064" x2="3.429" y2="0.6604" width="0.1524" layer="48"/>
<wire x1="3.175" y1="0.6604" x2="3.429" y2="0.6604" width="0.1524" layer="48"/>
<wire x1="3.302" y1="-0.4064" x2="3.175" y2="-0.6604" width="0.1524" layer="48"/>
<wire x1="3.302" y1="-0.4064" x2="3.429" y2="-0.6604" width="0.1524" layer="48"/>
<wire x1="3.175" y1="-0.6604" x2="3.429" y2="-0.6604" width="0.1524" layer="48"/>
<wire x1="-0.8128" y1="-0.4064" x2="-0.8128" y2="-2.9464" width="0.1524" layer="48"/>
<wire x1="-0.8128" y1="-2.9464" x2="-0.8128" y2="-3.3274" width="0.1524" layer="48"/>
<wire x1="0.8128" y1="-0.4064" x2="0.8128" y2="-2.9464" width="0.1524" layer="48"/>
<wire x1="0.8128" y1="-2.9464" x2="0.8128" y2="-3.3274" width="0.1524" layer="48"/>
<wire x1="-0.8128" y1="-2.9464" x2="-2.0828" y2="-2.9464" width="0.1524" layer="48"/>
<wire x1="0.8128" y1="-2.9464" x2="2.0828" y2="-2.9464" width="0.1524" layer="48"/>
<wire x1="-0.8128" y1="-2.9464" x2="-1.0668" y2="-2.8194" width="0.1524" layer="48"/>
<wire x1="-0.8128" y1="-2.9464" x2="-1.0668" y2="-3.0734" width="0.1524" layer="48"/>
<wire x1="-1.0668" y1="-2.8194" x2="-1.0668" y2="-3.0734" width="0.1524" layer="48"/>
<wire x1="0.8128" y1="-2.9464" x2="1.0668" y2="-2.8194" width="0.1524" layer="48"/>
<wire x1="0.8128" y1="-2.9464" x2="1.0668" y2="-3.0734" width="0.1524" layer="48"/>
<wire x1="1.0668" y1="-2.8194" x2="1.0668" y2="-3.0734" width="0.1524" layer="48"/>
<text x="-14.4272" y="-8.0264" size="1.27" layer="48" ratio="6" rot="SR0">Pin 1 Padstyle: RX32Y32D0T</text>
<text x="-16.9418" y="-9.5504" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX32Y32D0TSM2</text>
<text x="-14.8082" y="-14.1224" size="1.27" layer="48" ratio="6" rot="SR0">Alt 1 Padstyle: OX60Y90D30P</text>
<text x="-14.8082" y="-15.6464" size="1.27" layer="48" ratio="6" rot="SR0">Alt 2 Padstyle: OX90Y60D30P</text>
<text x="-3.4544" y="3.048" size="0.635" layer="48" ratio="4" rot="SR0">0.059in/1.5mm</text>
<text x="-7.6454" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0in/0mm</text>
<text x="3.81" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.032in/0.813mm</text>
<text x="-3.4544" y="-4.0894" size="0.635" layer="48" ratio="4" rot="SR0">0.063in/1.6mm</text>
<wire x1="-1.5494" y1="0" x2="-1.8034" y2="0" width="0.1524" layer="21" curve="-180"/>
<wire x1="-1.8034" y1="0" x2="-1.5494" y2="0" width="0.1524" layer="21" curve="-180"/>
<wire x1="-0.8128" y1="-0.4064" x2="0.8128" y2="-0.4064" width="0.1524" layer="51"/>
<wire x1="0.8128" y1="-0.4064" x2="0.8128" y2="0.4064" width="0.1524" layer="51"/>
<wire x1="0.8128" y1="0.4064" x2="0.3048" y2="0.4064" width="0.1524" layer="51"/>
<wire x1="0.3048" y1="0.4064" x2="-0.3048" y2="0.4064" width="0.1524" layer="51"/>
<wire x1="-0.3048" y1="0.4064" x2="-0.8128" y2="0.4064" width="0.1524" layer="51"/>
<wire x1="-0.8128" y1="0.4064" x2="-0.8128" y2="-0.4064" width="0.1524" layer="51"/>
<wire x1="-0.2794" y1="0" x2="-0.4318" y2="0" width="0" layer="51" curve="-180"/>
<wire x1="-0.4318" y1="0" x2="-0.2794" y2="0" width="0" layer="51" curve="-180"/>
<wire x1="0.3048" y1="0.4064" x2="-0.3048" y2="0.4064" width="0.1524" layer="51" curve="-180"/>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="SOT-23_TOR">
<smd name="1" x="-0.95" y="-1.1938" dx="0.4826" dy="1.1176" layer="1"/>
<smd name="2" x="0.95" y="-1.1938" dx="0.4826" dy="1.1176" layer="1"/>
<smd name="3" x="0" y="1.1938" dx="0.4826" dy="1.1176" layer="1"/>
<wire x1="-0.7366" y1="-0.8128" x2="-1.1684" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="-1.1684" y1="-0.8128" x2="-1.1684" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="-1.1684" y1="-1.397" x2="-0.7366" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="-0.7366" y1="-1.397" x2="-0.7366" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="1.1684" y1="-0.8128" x2="0.7366" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="0.7366" y1="-0.8128" x2="0.7366" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="0.7366" y1="-1.397" x2="1.1684" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="1.1684" y1="-1.397" x2="1.1684" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="-0.2032" y1="0.8128" x2="0.2032" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="0.2032" y1="0.8128" x2="0.2032" y2="1.397" width="0.1524" layer="51"/>
<wire x1="0.2032" y1="1.397" x2="-0.2286" y2="1.397" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="1.397" x2="-0.2032" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="-1.4478" y1="-0.8128" x2="1.4478" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="1.4478" y1="-0.8128" x2="1.4478" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="1.4478" y1="0.8128" x2="-1.4478" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="-1.4478" y1="0.8128" x2="-1.4478" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="5.2578" y1="1.397" x2="5.6388" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0" y1="-1.397" x2="0.9398" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-1.397" x2="5.2578" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.6388" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.2578" y2="2.667" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.2578" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.1308" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.3848" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.1308" y1="1.651" x2="5.3848" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.1308" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.3848" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="5.1308" y1="-1.651" x2="5.3848" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="0" y1="1.397" x2="0.2032" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="1.397" x2="3.3528" y2="1.397" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="5.2578" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0" y1="0.9906" x2="3.3528" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.7338" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.3528" y2="2.667" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.3528" y2="-0.2794" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.2258" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.4798" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.2258" y1="1.651" x2="3.4798" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.2258" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.4798" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.2258" y1="0.7366" x2="3.4798" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-1.397" x2="-0.9398" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-0.9398" y2="-4.318" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-1.397" x2="0.9398" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="0.9398" y2="-4.318" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-2.2098" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="2.2098" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-1.1938" y2="-3.81" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="-1.1938" y1="-3.81" x2="-1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="1.1938" y2="-3.81" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="1.1938" y1="-3.81" x2="1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="1.397" x2="-0.2032" y2="3.937" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.2032" y2="4.318" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="1.397" x2="0.2032" y2="3.937" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.2032" y2="4.318" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-1.4732" y2="3.937" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="1.4732" y2="3.937" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.4572" y2="4.064" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="-0.4572" y1="4.064" x2="-0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.4572" y2="4.064" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="0.4572" y1="4.064" x2="0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="0.8128" x2="-1.4478" y2="6.477" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.4478" y2="6.858" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="0.8128" x2="1.4478" y2="6.477" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.4478" y2="6.858" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-2.7178" y2="6.477" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="2.7178" y2="6.477" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.7018" y2="6.604" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="-1.7018" y1="6.604" x2="-1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.7018" y2="6.604" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="1.7018" y1="6.604" x2="1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="0" y1="0.8128" x2="-3.9878" y2="0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-4.3688" y2="0.8128" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.8128" x2="-3.9878" y2="-0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-4.3688" y2="-0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-3.9878" y2="2.0828" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-3.9878" y2="-2.0828" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-4.1148" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-3.8608" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-4.1148" y1="1.0668" x2="-3.8608" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-4.1148" y2="-1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-3.8608" y2="-1.0668" width="0.1524" layer="48"/>
<wire x1="-4.1148" y1="-1.0668" x2="-3.8608" y2="-1.0668" width="0.1524" layer="48"/>
<text x="-13.462" y="-8.1026" size="1.27" layer="48" ratio="6" rot="SR0">Top Padstyle: RX19Y44D0T</text>
<text x="-14.8082" y="-10.0076" size="1.27" layer="48" ratio="6" rot="SR0">Bottom Padstyle: RX19Y44D0T</text>
<text x="-14.8082" y="-11.9126" size="1.27" layer="48" ratio="6" rot="SR0">Alt 1 Padstyle: OX60Y90D30P</text>
<text x="-14.8082" y="-13.8176" size="1.27" layer="48" ratio="6" rot="SR0">Alt 2 Padstyle: OX90Y60D30P</text>
<text x="5.7658" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.11in/2.794mm</text>
<text x="3.8608" y="0.8636" size="0.635" layer="48" ratio="4" rot="SR0">0.016in/0.406mm</text>
<text x="-3.4544" y="-5.08" size="0.635" layer="48" ratio="4" rot="SR0">0.075in/1.9mm</text>
<text x="-4.0386" y="4.445" size="0.635" layer="48" ratio="4" rot="SR0">0.017in/0.432mm</text>
<text x="-4.0386" y="6.985" size="0.635" layer="48" ratio="4" rot="SR0">0.114in/2.896mm</text>
<text x="-12.573" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.065in/1.651mm</text>
<wire x1="-0.381" y1="-0.9652" x2="0.381" y2="-0.9652" width="0.1524" layer="21"/>
<wire x1="1.5748" y1="-0.9652" x2="1.5748" y2="0.9652" width="0.1524" layer="21"/>
<wire x1="1.5748" y1="0.9652" x2="0.5842" y2="0.9652" width="0.1524" layer="21"/>
<wire x1="-1.5748" y1="0.9652" x2="-1.5748" y2="-0.9652" width="0.1524" layer="21"/>
<wire x1="-0.5842" y1="0.9652" x2="-1.5748" y2="0.9652" width="0.1524" layer="21"/>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="SOT-23_TOR-M">
<smd name="1" x="-0.95" y="-1.2446" dx="0.5334" dy="1.4224" layer="1"/>
<smd name="2" x="0.95" y="-1.2446" dx="0.5334" dy="1.4224" layer="1"/>
<smd name="3" x="0" y="1.2446" dx="0.5334" dy="1.4224" layer="1"/>
<wire x1="-0.7366" y1="-0.8128" x2="-1.1684" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="-1.1684" y1="-0.8128" x2="-1.1684" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="-1.1684" y1="-1.397" x2="-0.7366" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="-0.7366" y1="-1.397" x2="-0.7366" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="1.1684" y1="-0.8128" x2="0.7366" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="0.7366" y1="-0.8128" x2="0.7366" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="0.7366" y1="-1.397" x2="1.1684" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="1.1684" y1="-1.397" x2="1.1684" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="-0.2032" y1="0.8128" x2="0.2032" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="0.2032" y1="0.8128" x2="0.2032" y2="1.397" width="0.1524" layer="51"/>
<wire x1="0.2032" y1="1.397" x2="-0.2286" y2="1.397" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="1.397" x2="-0.2032" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="-1.4478" y1="-0.8128" x2="1.4478" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="1.4478" y1="-0.8128" x2="1.4478" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="1.4478" y1="0.8128" x2="-1.4478" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="-1.4478" y1="0.8128" x2="-1.4478" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="-0.8636" y1="-0.5588" x2="-1.016" y2="-0.5842" width="0" layer="51" curve="-180"/>
<wire x1="-1.016" y1="-0.5842" x2="-0.8636" y2="-0.5588" width="0" layer="51" curve="-180"/>
<wire x1="5.2578" y1="1.397" x2="5.6388" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0" y1="-1.397" x2="0.9398" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-1.397" x2="5.2578" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.6388" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.2578" y2="2.667" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.2578" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.1308" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.3848" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.1308" y1="1.651" x2="5.3848" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.1308" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.3848" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="5.1308" y1="-1.651" x2="5.3848" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="0" y1="1.397" x2="0.2032" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="1.397" x2="3.3528" y2="1.397" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="5.2578" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0" y1="0.9906" x2="3.3528" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.7338" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.3528" y2="2.667" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.3528" y2="-0.2794" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.2258" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.4798" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.2258" y1="1.651" x2="3.4798" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.2258" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.4798" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.2258" y1="0.7366" x2="3.4798" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-1.397" x2="-0.9398" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-0.9398" y2="-4.318" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-1.397" x2="0.9398" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="0.9398" y2="-4.318" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-2.2098" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="2.2098" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-1.1938" y2="-3.81" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="-1.1938" y1="-3.81" x2="-1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="1.1938" y2="-3.81" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="1.1938" y1="-3.81" x2="1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="1.397" x2="-0.2032" y2="3.937" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.2032" y2="4.318" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="1.397" x2="0.2032" y2="3.937" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.2032" y2="4.318" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-1.4732" y2="3.937" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="1.4732" y2="3.937" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.4572" y2="4.064" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="-0.4572" y1="4.064" x2="-0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.4572" y2="4.064" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="0.4572" y1="4.064" x2="0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="0.8128" x2="-1.4478" y2="6.477" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.4478" y2="6.858" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="0.8128" x2="1.4478" y2="6.477" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.4478" y2="6.858" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-2.7178" y2="6.477" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="2.7178" y2="6.477" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.7018" y2="6.604" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="-1.7018" y1="6.604" x2="-1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.7018" y2="6.604" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="1.7018" y1="6.604" x2="1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="0" y1="0.8128" x2="-3.9878" y2="0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-4.3688" y2="0.8128" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.8128" x2="-3.9878" y2="-0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-4.3688" y2="-0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-3.9878" y2="2.0828" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-3.9878" y2="-2.0828" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-4.1148" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-3.8608" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-4.1148" y1="1.0668" x2="-3.8608" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-4.1148" y2="-1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-3.8608" y2="-1.0668" width="0.1524" layer="48"/>
<wire x1="-4.1148" y1="-1.0668" x2="-3.8608" y2="-1.0668" width="0.1524" layer="48"/>
<text x="-13.462" y="-8.3058" size="1.27" layer="48" ratio="6" rot="SR0">Top Padstyle: RX21Y56D0T</text>
<text x="-14.8082" y="-10.2108" size="1.27" layer="48" ratio="6" rot="SR0">Bottom Padstyle: RX21Y56D0T</text>
<text x="-14.8082" y="-12.1158" size="1.27" layer="48" ratio="6" rot="SR0">Alt 1 Padstyle: OX60Y90D30P</text>
<text x="-14.8082" y="-14.0208" size="1.27" layer="48" ratio="6" rot="SR0">Alt 2 Padstyle: OX90Y60D30P</text>
<text x="5.7658" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.11in/2.794mm</text>
<text x="3.8608" y="0.8636" size="0.635" layer="48" ratio="4" rot="SR0">0.016in/0.406mm</text>
<text x="-3.4544" y="-5.08" size="0.635" layer="48" ratio="4" rot="SR0">0.075in/1.9mm</text>
<text x="-4.0386" y="4.445" size="0.635" layer="48" ratio="4" rot="SR0">0.017in/0.432mm</text>
<text x="-4.0386" y="6.985" size="0.635" layer="48" ratio="4" rot="SR0">0.114in/2.896mm</text>
<text x="-12.573" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.065in/1.651mm</text>
<wire x1="-0.3556" y1="-0.9652" x2="0.3556" y2="-0.9652" width="0.1524" layer="21"/>
<wire x1="1.5748" y1="-0.9652" x2="1.5748" y2="0.9652" width="0.1524" layer="21"/>
<wire x1="1.5748" y1="0.9652" x2="0.6096" y2="0.9652" width="0.1524" layer="21"/>
<wire x1="-1.5748" y1="0.9652" x2="-1.5748" y2="-0.9652" width="0.1524" layer="21"/>
<wire x1="-0.6096" y1="0.9652" x2="-1.5748" y2="0.9652" width="0.1524" layer="21"/>
<wire x1="-0.8636" y1="-2.7178" x2="-1.016" y2="-2.7178" width="0.1524" layer="21" curve="-180"/>
<wire x1="-1.016" y1="-2.7178" x2="-0.8636" y2="-2.7178" width="0.1524" layer="21" curve="-180"/>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="SOT-23_TOR-L">
<smd name="1" x="-0.95" y="-1.143" dx="0.4318" dy="0.8128" layer="1"/>
<smd name="2" x="0.95" y="-1.143" dx="0.4318" dy="0.8128" layer="1"/>
<smd name="3" x="0" y="1.143" dx="0.4318" dy="0.8128" layer="1"/>
<wire x1="-0.7366" y1="-0.8128" x2="-1.1684" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="-1.1684" y1="-0.8128" x2="-1.1684" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="-1.1684" y1="-1.397" x2="-0.7366" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="-0.7366" y1="-1.397" x2="-0.7366" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="1.1684" y1="-0.8128" x2="0.7366" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="0.7366" y1="-0.8128" x2="0.7366" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="0.7366" y1="-1.397" x2="1.1684" y2="-1.397" width="0.1524" layer="51"/>
<wire x1="1.1684" y1="-1.397" x2="1.1684" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="-0.2032" y1="0.8128" x2="0.2032" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="0.2032" y1="0.8128" x2="0.2032" y2="1.397" width="0.1524" layer="51"/>
<wire x1="0.2032" y1="1.397" x2="-0.2286" y2="1.397" width="0.1524" layer="51"/>
<wire x1="-0.2286" y1="1.397" x2="-0.2032" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="-1.4478" y1="-0.8128" x2="1.4478" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="1.4478" y1="-0.8128" x2="1.4478" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="1.4478" y1="0.8128" x2="-1.4478" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="-1.4478" y1="0.8128" x2="-1.4478" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="-0.8636" y1="-0.5588" x2="-1.016" y2="-0.5842" width="0" layer="51" curve="-180"/>
<wire x1="-1.016" y1="-0.5842" x2="-0.8636" y2="-0.5588" width="0" layer="51" curve="-180"/>
<wire x1="5.2578" y1="1.397" x2="5.6388" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0" y1="-1.397" x2="0.9398" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-1.397" x2="5.2578" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.6388" y2="-1.397" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.2578" y2="2.667" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.2578" y2="-2.667" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.1308" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="1.397" x2="5.3848" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.1308" y1="1.651" x2="5.3848" y2="1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.1308" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="5.2578" y1="-1.397" x2="5.3848" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="5.1308" y1="-1.651" x2="5.3848" y2="-1.651" width="0.1524" layer="48"/>
<wire x1="0" y1="1.397" x2="0.2032" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="1.397" x2="3.3528" y2="1.397" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="5.2578" y2="1.397" width="0.1524" layer="48"/>
<wire x1="0" y1="0.9906" x2="3.3528" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.7338" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.3528" y2="2.667" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.3528" y2="-0.2794" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.2258" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="1.397" x2="3.4798" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.2258" y1="1.651" x2="3.4798" y2="1.651" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.2258" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.3528" y1="0.9906" x2="3.4798" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.2258" y1="0.7366" x2="3.4798" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-1.397" x2="-0.9398" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-0.9398" y2="-4.318" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-1.397" x2="0.9398" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="0.9398" y2="-4.318" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-2.2098" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="2.2098" y2="-3.937" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-1.1938" y2="-3.81" width="0.1524" layer="48"/>
<wire x1="-0.9398" y1="-3.937" x2="-1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="-1.1938" y1="-3.81" x2="-1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="1.1938" y2="-3.81" width="0.1524" layer="48"/>
<wire x1="0.9398" y1="-3.937" x2="1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="1.1938" y1="-3.81" x2="1.1938" y2="-4.064" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="1.397" x2="-0.2032" y2="3.937" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.2032" y2="4.318" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="1.397" x2="0.2032" y2="3.937" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.2032" y2="4.318" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-1.4732" y2="3.937" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="1.4732" y2="3.937" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.4572" y2="4.064" width="0.1524" layer="48"/>
<wire x1="-0.2032" y1="3.937" x2="-0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="-0.4572" y1="4.064" x2="-0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.4572" y2="4.064" width="0.1524" layer="48"/>
<wire x1="0.2032" y1="3.937" x2="0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="0.4572" y1="4.064" x2="0.4572" y2="3.81" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="0.8128" x2="-1.4478" y2="6.477" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.4478" y2="6.858" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="0.8128" x2="1.4478" y2="6.477" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.4478" y2="6.858" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-2.7178" y2="6.477" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="2.7178" y2="6.477" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.7018" y2="6.604" width="0.1524" layer="48"/>
<wire x1="-1.4478" y1="6.477" x2="-1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="-1.7018" y1="6.604" x2="-1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.7018" y2="6.604" width="0.1524" layer="48"/>
<wire x1="1.4478" y1="6.477" x2="1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="1.7018" y1="6.604" x2="1.7018" y2="6.35" width="0.1524" layer="48"/>
<wire x1="0" y1="0.8128" x2="-3.9878" y2="0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-4.3688" y2="0.8128" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.8128" x2="-3.9878" y2="-0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-4.3688" y2="-0.8128" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-3.9878" y2="2.0828" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-3.9878" y2="-2.0828" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-4.1148" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="0.8128" x2="-3.8608" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-4.1148" y1="1.0668" x2="-3.8608" y2="1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-4.1148" y2="-1.0668" width="0.1524" layer="48"/>
<wire x1="-3.9878" y1="-0.8128" x2="-3.8608" y2="-1.0668" width="0.1524" layer="48"/>
<wire x1="-4.1148" y1="-1.0668" x2="-3.8608" y2="-1.0668" width="0.1524" layer="48"/>
<text x="-13.462" y="-7.8994" size="1.27" layer="48" ratio="6" rot="SR0">Top Padstyle: RX17Y32D0T</text>
<text x="-14.8082" y="-9.8044" size="1.27" layer="48" ratio="6" rot="SR0">Bottom Padstyle: RX17Y32D0T</text>
<text x="-14.8082" y="-11.7094" size="1.27" layer="48" ratio="6" rot="SR0">Alt 1 Padstyle: OX60Y90D30P</text>
<text x="-14.8082" y="-13.6144" size="1.27" layer="48" ratio="6" rot="SR0">Alt 2 Padstyle: OX90Y60D30P</text>
<text x="5.7658" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.11in/2.794mm</text>
<text x="3.8608" y="0.8636" size="0.635" layer="48" ratio="4" rot="SR0">0.016in/0.406mm</text>
<text x="-3.4544" y="-5.08" size="0.635" layer="48" ratio="4" rot="SR0">0.075in/1.9mm</text>
<text x="-4.0386" y="4.445" size="0.635" layer="48" ratio="4" rot="SR0">0.017in/0.432mm</text>
<text x="-4.0386" y="6.985" size="0.635" layer="48" ratio="4" rot="SR0">0.114in/2.896mm</text>
<text x="-12.573" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.065in/1.651mm</text>
<wire x1="-0.4064" y1="-0.9652" x2="0.4064" y2="-0.9652" width="0.1524" layer="21"/>
<wire x1="1.5748" y1="-0.9652" x2="1.5748" y2="0.9652" width="0.1524" layer="21"/>
<wire x1="1.5748" y1="0.9652" x2="0.5588" y2="0.9652" width="0.1524" layer="21"/>
<wire x1="-1.5748" y1="0.9652" x2="-1.5748" y2="-0.9652" width="0.1524" layer="21"/>
<wire x1="-0.5588" y1="0.9652" x2="-1.5748" y2="0.9652" width="0.1524" layer="21"/>
<wire x1="-0.8636" y1="-2.3114" x2="-1.016" y2="-2.3114" width="0.1524" layer="21" curve="-180"/>
<wire x1="-1.016" y1="-2.3114" x2="-0.8636" y2="-2.3114" width="0.1524" layer="21" curve="-180"/>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
</packages>
<symbols>
<symbol name="ADS131M08PBS">
<pin name="AIN2P" x="-25.4" y="22.86" length="middle" direction="in"/>
<pin name="AIN2N" x="-25.4" y="20.32" length="middle" direction="in"/>
<pin name="AIN3N" x="-25.4" y="15.24" length="middle" direction="in"/>
<pin name="AIN3P" x="-25.4" y="12.7" length="middle" direction="in"/>
<pin name="AIN4P" x="-25.4" y="7.62" length="middle" direction="in"/>
<pin name="AIN4N" x="-25.4" y="5.08" length="middle" direction="in"/>
<pin name="AIN5N" x="-25.4" y="0" length="middle" direction="in"/>
<pin name="AIN5P" x="-25.4" y="-2.54" length="middle" direction="in"/>
<pin name="AIN6P" x="-25.4" y="-7.62" length="middle" direction="in"/>
<pin name="AIN6N" x="-25.4" y="-10.16" length="middle" direction="in"/>
<pin name="AIN7N" x="-25.4" y="-15.24" length="middle" direction="in"/>
<pin name="AIN7P" x="-25.4" y="-17.78" length="middle" direction="in"/>
<pin name="AGND_REFGND" x="-25.4" y="-35.56" length="middle" direction="pwr"/>
<pin name="REFIN" x="-25.4" y="-30.48" length="middle" direction="in"/>
<pin name="AVDD_2" x="-25.4" y="-25.4" length="middle" direction="pwr"/>
<pin name="SYNC_RESET_N" x="25.4" y="-20.32" length="middle" direction="in" rot="R180"/>
<pin name="CS_N" x="25.4" y="-15.24" length="middle" direction="in" rot="R180"/>
<pin name="DRDY_N" x="25.4" y="-10.16" length="middle" direction="out" rot="R180"/>
<pin name="SCLK" x="25.4" y="-5.08" length="middle" direction="in" rot="R180"/>
<pin name="DOUT" x="25.4" y="0" length="middle" direction="out" rot="R180"/>
<pin name="DIN" x="25.4" y="5.08" length="middle" direction="in" rot="R180"/>
<pin name="XTAL2" x="25.4" y="17.78" length="middle" direction="in" rot="R180"/>
<pin name="XTAL1_CLKIN" x="25.4" y="25.4" length="middle" direction="in" rot="R180"/>
<pin name="LDOCAP" x="25.4" y="33.02" length="middle" direction="pas" rot="R180"/>
<pin name="DGND" x="25.4" y="-40.64" length="middle" direction="pwr" rot="R180"/>
<pin name="IOVDD" x="25.4" y="-33.02" length="middle" direction="pwr" rot="R180"/>
<pin name="AVDD" x="-25.4" y="-22.86" length="middle" direction="pwr"/>
<pin name="AGND" x="-25.4" y="-40.64" length="middle" direction="pwr"/>
<pin name="AIN0P" x="-25.4" y="38.1" length="middle" direction="in"/>
<pin name="AIN0N" x="-25.4" y="35.56" length="middle" direction="in"/>
<pin name="AIN1N" x="-25.4" y="30.48" length="middle" direction="in"/>
<pin name="AIN1P" x="-25.4" y="27.94" length="middle" direction="in"/>
<wire x1="-20.32" y1="-43.18" x2="20.32" y2="-43.18" width="0.2032" layer="94"/>
<wire x1="20.32" y1="-43.18" x2="20.32" y2="43.18" width="0.2032" layer="94"/>
<wire x1="20.32" y1="43.18" x2="-20.32" y2="43.18" width="0.2032" layer="94"/>
<wire x1="-20.32" y1="43.18" x2="-20.32" y2="-43.18" width="0.2032" layer="94"/>
<text x="-4.7244" y="1.4986" size="2.0828" layer="95" ratio="6" rot="SR0">&gt;Name</text>
<text x="-5.3594" y="-1.0414" size="2.0828" layer="96" ratio="6" rot="SR0">&gt;Value</text>
<text x="-5.3594" y="-1.0414" size="2.0828" layer="96" ratio="6" rot="SR0">&gt;Value</text>
</symbol>
<symbol name="RES">
<pin name="2" x="0" y="0" visible="pin" length="short" direction="pas" swaplevel="1"/>
<pin name="1" x="12.7" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
<text x="-2.6162" y="-5.5372" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.6162" y="-8.7376" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.1844" y="2.0828" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
<text x="-2.6162" y="-5.5372" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.6162" y="-8.7376" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.1844" y="2.0828" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
<text x="-2.6162" y="-5.5372" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.6162" y="-8.7376" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.1844" y="2.0828" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
<wire x1="3.175" y1="1.27" x2="4.445" y2="-1.27" width="0.2032" layer="94"/>
<wire x1="4.445" y1="-1.27" x2="5.715" y2="1.27" width="0.2032" layer="94"/>
<wire x1="5.715" y1="1.27" x2="6.985" y2="-1.27" width="0.2032" layer="94"/>
<wire x1="6.985" y1="-1.27" x2="8.255" y2="1.27" width="0.2032" layer="94"/>
<wire x1="8.255" y1="1.27" x2="9.525" y2="-1.27" width="0.2032" layer="94"/>
<wire x1="2.54" y1="0" x2="3.175" y2="1.27" width="0.2032" layer="94"/>
<wire x1="9.525" y1="-1.27" x2="10.16" y2="0" width="0.2032" layer="94"/>
<text x="-2.6162" y="-5.5372" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.6162" y="-8.7376" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.1844" y="2.0828" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
</symbol>
<symbol name="CONN_08P_00C_1">
<pin name="1" x="0" y="0" visible="pad" length="middle" direction="pas"/>
<pin name="2" x="0" y="-2.54" visible="pad" length="middle" direction="pas"/>
<pin name="3" x="0" y="-5.08" visible="pad" length="middle" direction="pas"/>
<pin name="4" x="0" y="-7.62" visible="pad" length="middle" direction="pas"/>
<pin name="5" x="0" y="-10.16" visible="pad" length="middle" direction="pas"/>
<pin name="6" x="0" y="-12.7" visible="pad" length="middle" direction="pas"/>
<pin name="7" x="0" y="-15.24" visible="pad" length="middle" direction="pas"/>
<pin name="8" x="0" y="-17.78" visible="pad" length="middle" direction="pas"/>
<wire x1="10.16" y1="0" x2="5.08" y2="0" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-2.54" x2="5.08" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-5.08" x2="5.08" y2="-5.08" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-7.62" x2="5.08" y2="-7.62" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-10.16" x2="5.08" y2="-10.16" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-12.7" x2="5.08" y2="-12.7" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-15.24" x2="5.08" y2="-15.24" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-17.78" x2="5.08" y2="-17.78" width="0.1524" layer="94"/>
<wire x1="10.16" y1="0" x2="8.89" y2="0.8382" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-2.54" x2="8.89" y2="-1.7018" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-5.08" x2="8.89" y2="-4.2418" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-7.62" x2="8.89" y2="-6.7818" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-10.16" x2="8.89" y2="-9.3218" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-12.7" x2="8.89" y2="-11.8618" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-15.24" x2="8.89" y2="-14.4018" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-17.78" x2="8.89" y2="-16.9418" width="0.1524" layer="94"/>
<wire x1="10.16" y1="0" x2="8.89" y2="-0.8382" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-2.54" x2="8.89" y2="-3.3782" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-5.08" x2="8.89" y2="-5.9182" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-7.62" x2="8.89" y2="-8.4582" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-10.16" x2="8.89" y2="-10.9982" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-12.7" x2="8.89" y2="-13.5382" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-15.24" x2="8.89" y2="-16.0782" width="0.1524" layer="94"/>
<wire x1="10.16" y1="-17.78" x2="8.89" y2="-18.6182" width="0.1524" layer="94"/>
<wire x1="5.08" y1="2.54" x2="5.08" y2="-20.32" width="0.1524" layer="94"/>
<wire x1="5.08" y1="-20.32" x2="12.7" y2="-20.32" width="0.1524" layer="94"/>
<wire x1="12.7" y1="-20.32" x2="12.7" y2="2.54" width="0.1524" layer="94"/>
<wire x1="12.7" y1="2.54" x2="5.08" y2="2.54" width="0.1524" layer="94"/>
<text x="4.1656" y="5.3086" size="2.0828" layer="95" ratio="6" rot="SR0">&gt;Name</text>
<text x="4.8006" y="-11.2014" size="2.0828" layer="96" ratio="6" rot="SR0">&gt;Value</text>
</symbol>
<symbol name="LED">
<pin name="ANODE" x="0" y="0" visible="off" length="short" direction="pas"/>
<pin name="CATHODE" x="10.16" y="0" visible="off" length="short" direction="pas" rot="R180"/>
<wire x1="3.81" y1="1.905" x2="3.81" y2="0" width="0.2032" layer="94"/>
<wire x1="3.81" y1="0" x2="3.81" y2="-1.905" width="0.2032" layer="94"/>
<wire x1="2.54" y1="0" x2="3.81" y2="0" width="0.2032" layer="94"/>
<wire x1="6.35" y1="-1.905" x2="6.35" y2="0" width="0.2032" layer="94"/>
<wire x1="6.35" y1="0" x2="6.35" y2="1.905" width="0.2032" layer="94"/>
<wire x1="6.35" y1="0" x2="7.62" y2="0" width="0.2032" layer="94"/>
<wire x1="6.35" y1="4.445" x2="6.985" y2="3.81" width="0.2032" layer="94"/>
<wire x1="6.985" y1="3.81" x2="8.255" y2="5.08" width="0.2032" layer="94"/>
<wire x1="8.255" y1="3.81" x2="8.89" y2="3.175" width="0.2032" layer="94"/>
<wire x1="8.89" y1="3.175" x2="10.16" y2="4.445" width="0.2032" layer="94"/>
<wire x1="8.255" y1="5.08" x2="7.62" y2="5.08" width="0.2032" layer="94"/>
<wire x1="7.62" y1="5.08" x2="8.255" y2="4.445" width="0.2032" layer="94"/>
<wire x1="8.255" y1="4.445" x2="8.255" y2="5.08" width="0.2032" layer="94"/>
<wire x1="10.16" y1="4.445" x2="9.525" y2="4.445" width="0.2032" layer="94"/>
<wire x1="9.525" y1="4.445" x2="10.16" y2="3.81" width="0.2032" layer="94"/>
<wire x1="10.16" y1="3.81" x2="10.16" y2="4.445" width="0.2032" layer="94"/>
<wire x1="6.985" y1="2.54" x2="8.255" y2="3.81" width="0.2032" layer="94"/>
<wire x1="6.35" y1="0" x2="3.81" y2="1.905" width="0.2032" layer="94"/>
<wire x1="3.81" y1="-1.905" x2="6.35" y2="0" width="0.2032" layer="94"/>
<wire x1="5.08" y1="3.175" x2="6.35" y2="4.445" width="0.2032" layer="94"/>
<text x="-3.8862" y="-9.3472" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-3.8862" y="-9.3472" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-2.8194" y="-6.1976" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
</symbol>
<symbol name="SOT-23">
<pin name="VIN" x="0" y="0" direction="pwr"/>
<pin name="VOUT" x="40.64" y="0" direction="out" rot="R180"/>
<pin name="VSS" x="20.32" y="-20.32" direction="pas" rot="R90"/>
<wire x1="7.62" y1="5.08" x2="7.62" y2="-12.7" width="0.1524" layer="94"/>
<wire x1="7.62" y1="-12.7" x2="33.02" y2="-12.7" width="0.1524" layer="94"/>
<wire x1="33.02" y1="-12.7" x2="33.02" y2="5.08" width="0.1524" layer="94"/>
<wire x1="33.02" y1="5.08" x2="7.62" y2="5.08" width="0.1524" layer="94"/>
<text x="15.5956" y="9.1186" size="2.0828" layer="95" ratio="6" rot="SR0">&gt;Name</text>
<text x="14.9606" y="6.5786" size="2.0828" layer="96" ratio="6" rot="SR0">&gt;Value</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="ADS131M08IPBS" prefix="U">
<gates>
<gate name="A" symbol="ADS131M08PBS" x="0" y="0"/>
</gates>
<devices>
<device name="" package="PBS0032A_N">
<connects>
<connect gate="A" pin="AGND" pad="28"/>
<connect gate="A" pin="AGND_REFGND" pad="13"/>
<connect gate="A" pin="AIN0N" pad="30"/>
<connect gate="A" pin="AIN0P" pad="29"/>
<connect gate="A" pin="AIN1N" pad="31"/>
<connect gate="A" pin="AIN1P" pad="32"/>
<connect gate="A" pin="AIN2N" pad="2"/>
<connect gate="A" pin="AIN2P" pad="1"/>
<connect gate="A" pin="AIN3N" pad="3"/>
<connect gate="A" pin="AIN3P" pad="4"/>
<connect gate="A" pin="AIN4N" pad="6"/>
<connect gate="A" pin="AIN4P" pad="5"/>
<connect gate="A" pin="AIN5N" pad="7"/>
<connect gate="A" pin="AIN5P" pad="8"/>
<connect gate="A" pin="AIN6N" pad="10"/>
<connect gate="A" pin="AIN6P" pad="9"/>
<connect gate="A" pin="AIN7N" pad="11"/>
<connect gate="A" pin="AIN7P" pad="12"/>
<connect gate="A" pin="AVDD" pad="27"/>
<connect gate="A" pin="AVDD_2" pad="15"/>
<connect gate="A" pin="CS_N" pad="17"/>
<connect gate="A" pin="DGND" pad="25"/>
<connect gate="A" pin="DIN" pad="21"/>
<connect gate="A" pin="DOUT" pad="20"/>
<connect gate="A" pin="DRDY_N" pad="18"/>
<connect gate="A" pin="IOVDD" pad="26"/>
<connect gate="A" pin="LDOCAP" pad="24"/>
<connect gate="A" pin="REFIN" pad="14"/>
<connect gate="A" pin="SCLK" pad="19"/>
<connect gate="A" pin="SYNC_RESET_N" pad="16"/>
<connect gate="A" pin="XTAL1_CLKIN" pad="23"/>
<connect gate="A" pin="XTAL2" pad="22"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="ADS131M08IPBS" constant="no"/>
<attribute name="MFR_NAME" value="Texas Instruments" constant="no"/>
<attribute name="REFDES" value="RefDes" constant="no"/>
<attribute name="TYPE" value="TYPE" constant="no"/>
</technology>
</technologies>
</device>
<device name="PBS0032A_M" package="PBS0032A_M">
<connects>
<connect gate="A" pin="AGND" pad="28"/>
<connect gate="A" pin="AGND_REFGND" pad="13"/>
<connect gate="A" pin="AIN0N" pad="30"/>
<connect gate="A" pin="AIN0P" pad="29"/>
<connect gate="A" pin="AIN1N" pad="31"/>
<connect gate="A" pin="AIN1P" pad="32"/>
<connect gate="A" pin="AIN2N" pad="2"/>
<connect gate="A" pin="AIN2P" pad="1"/>
<connect gate="A" pin="AIN3N" pad="3"/>
<connect gate="A" pin="AIN3P" pad="4"/>
<connect gate="A" pin="AIN4N" pad="6"/>
<connect gate="A" pin="AIN4P" pad="5"/>
<connect gate="A" pin="AIN5N" pad="7"/>
<connect gate="A" pin="AIN5P" pad="8"/>
<connect gate="A" pin="AIN6N" pad="10"/>
<connect gate="A" pin="AIN6P" pad="9"/>
<connect gate="A" pin="AIN7N" pad="11"/>
<connect gate="A" pin="AIN7P" pad="12"/>
<connect gate="A" pin="AVDD" pad="27"/>
<connect gate="A" pin="AVDD_2" pad="15"/>
<connect gate="A" pin="CS_N" pad="17"/>
<connect gate="A" pin="DGND" pad="25"/>
<connect gate="A" pin="DIN" pad="21"/>
<connect gate="A" pin="DOUT" pad="20"/>
<connect gate="A" pin="DRDY_N" pad="18"/>
<connect gate="A" pin="IOVDD" pad="26"/>
<connect gate="A" pin="LDOCAP" pad="24"/>
<connect gate="A" pin="REFIN" pad="14"/>
<connect gate="A" pin="SCLK" pad="19"/>
<connect gate="A" pin="SYNC_RESET_N" pad="16"/>
<connect gate="A" pin="XTAL1_CLKIN" pad="23"/>
<connect gate="A" pin="XTAL2" pad="22"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="ADS131M08IPBS" constant="no"/>
<attribute name="MFR_NAME" value="Texas Instruments" constant="no"/>
<attribute name="REFDES" value="RefDes" constant="no"/>
<attribute name="TYPE" value="TYPE" constant="no"/>
</technology>
</technologies>
</device>
<device name="PBS0032A_L" package="PBS0032A_L">
<connects>
<connect gate="A" pin="AGND" pad="28"/>
<connect gate="A" pin="AGND_REFGND" pad="13"/>
<connect gate="A" pin="AIN0N" pad="30"/>
<connect gate="A" pin="AIN0P" pad="29"/>
<connect gate="A" pin="AIN1N" pad="31"/>
<connect gate="A" pin="AIN1P" pad="32"/>
<connect gate="A" pin="AIN2N" pad="2"/>
<connect gate="A" pin="AIN2P" pad="1"/>
<connect gate="A" pin="AIN3N" pad="3"/>
<connect gate="A" pin="AIN3P" pad="4"/>
<connect gate="A" pin="AIN4N" pad="6"/>
<connect gate="A" pin="AIN4P" pad="5"/>
<connect gate="A" pin="AIN5N" pad="7"/>
<connect gate="A" pin="AIN5P" pad="8"/>
<connect gate="A" pin="AIN6N" pad="10"/>
<connect gate="A" pin="AIN6P" pad="9"/>
<connect gate="A" pin="AIN7N" pad="11"/>
<connect gate="A" pin="AIN7P" pad="12"/>
<connect gate="A" pin="AVDD" pad="27"/>
<connect gate="A" pin="AVDD_2" pad="15"/>
<connect gate="A" pin="CS_N" pad="17"/>
<connect gate="A" pin="DGND" pad="25"/>
<connect gate="A" pin="DIN" pad="21"/>
<connect gate="A" pin="DOUT" pad="20"/>
<connect gate="A" pin="DRDY_N" pad="18"/>
<connect gate="A" pin="IOVDD" pad="26"/>
<connect gate="A" pin="LDOCAP" pad="24"/>
<connect gate="A" pin="REFIN" pad="14"/>
<connect gate="A" pin="SCLK" pad="19"/>
<connect gate="A" pin="SYNC_RESET_N" pad="16"/>
<connect gate="A" pin="XTAL1_CLKIN" pad="23"/>
<connect gate="A" pin="XTAL2" pad="22"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="ADS131M08IPBS" constant="no"/>
<attribute name="MFR_NAME" value="Texas Instruments" constant="no"/>
<attribute name="REFDES" value="RefDes" constant="no"/>
<attribute name="TYPE" value="TYPE" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="RC0402JR-07150RL" prefix="R">
<gates>
<gate name="A" symbol="RES" x="0" y="0"/>
</gates>
<devices>
<device name="" package="RC0402N_YAG">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="RC0402JR-07150RL" constant="no"/>
<attribute name="MFR_NAME" value="Yageo" constant="no"/>
</technology>
</technologies>
</device>
<device name="RC0402N_YAG-M" package="RC0402N_YAG-M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="RC0402JR-07150RL" constant="no"/>
<attribute name="MFR_NAME" value="Yageo" constant="no"/>
</technology>
</technologies>
</device>
<device name="RC0402N_YAG-L" package="RC0402N_YAG-L">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="RC0402JR-07150RL" constant="no"/>
<attribute name="MFR_NAME" value="Yageo" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="282834-8" prefix="J">
<gates>
<gate name="A" symbol="CONN_08P_00C_1" x="0" y="0"/>
</gates>
<devices>
<device name="" package="CONN_282834-8_TEC">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="282834-8" constant="no"/>
<attribute name="MFR_NAME" value="TE Connectivity" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="RC0402JR-071KL" prefix="R">
<gates>
<gate name="A" symbol="RES" x="0" y="0"/>
</gates>
<devices>
<device name="" package="RC0402N_YAG">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="RC0402JR-071KL" constant="no"/>
<attribute name="MFR_NAME" value="Yageo" constant="no"/>
</technology>
</technologies>
</device>
<device name="RC0402N_YAG-M" package="RC0402N_YAG-M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="RC0402JR-071KL" constant="no"/>
<attribute name="MFR_NAME" value="Yageo" constant="no"/>
</technology>
</technologies>
</device>
<device name="RC0402N_YAG-L" package="RC0402N_YAG-L">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="RC0402JR-071KL" constant="no"/>
<attribute name="MFR_NAME" value="Yageo" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="LTST-C190KFKT" prefix="LED">
<gates>
<gate name="A" symbol="LED" x="0" y="0"/>
</gates>
<devices>
<device name="" package="LED_LSTS-C190_LTO">
<connects>
<connect gate="A" pin="ANODE" pad="2"/>
<connect gate="A" pin="CATHODE" pad="1"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="LTST-C190KFKT" constant="no"/>
<attribute name="MFR_NAME" value="LITE-ON" constant="no"/>
<attribute name="TYPE" value="LED Indication" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="XC6206P332MR-G" prefix="U">
<gates>
<gate name="A" symbol="SOT-23" x="0" y="0"/>
</gates>
<devices>
<device name="" package="SOT-23_TOR">
<connects>
<connect gate="A" pin="VIN" pad="3"/>
<connect gate="A" pin="VOUT" pad="2"/>
<connect gate="A" pin="VSS" pad="1"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="XC6206P332MR-G" constant="no"/>
<attribute name="MFR_NAME" value="Torex Semiconductor" constant="no"/>
</technology>
</technologies>
</device>
<device name="SOT-23_TOR-M" package="SOT-23_TOR-M">
<connects>
<connect gate="A" pin="VIN" pad="3"/>
<connect gate="A" pin="VOUT" pad="2"/>
<connect gate="A" pin="VSS" pad="1"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="XC6206P332MR-G" constant="no"/>
<attribute name="MFR_NAME" value="Torex Semiconductor" constant="no"/>
</technology>
</technologies>
</device>
<device name="SOT-23_TOR-L" package="SOT-23_TOR-L">
<connects>
<connect gate="A" pin="VIN" pad="3"/>
<connect gate="A" pin="VOUT" pad="2"/>
<connect gate="A" pin="VSS" pad="1"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="XC6206P332MR-G" constant="no"/>
<attribute name="MFR_NAME" value="Torex Semiconductor" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="C3225X7R1N106M250AC">
<packages>
<package name="CAPC220145_140N_KEM">
<smd name="1" x="-0.8028" y="0" dx="1.305596875" dy="1.449996875" layer="1"/>
<smd name="2" x="0.8028" y="0" dx="1.305596875" dy="1.449996875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.5946" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX51p4Y57p09D0T</text>
<text x="-18.0848" y="-10.1346" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.6746" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.0254" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.0254" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="CAPC220145_140N_KEM-M">
<smd name="1" x="-0.9044" y="0" dx="1.508796875" dy="1.500796875" layer="1"/>
<smd name="2" x="0.9044" y="0" dx="1.508796875" dy="1.500796875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.62" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX59p4Y59p09D0T</text>
<text x="-18.0848" y="-10.16" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.7" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="CAPC220145_140N_KEM-L">
<smd name="1" x="-0.7012" y="0" dx="1.102396875" dy="1.399196875" layer="1"/>
<smd name="2" x="0.7012" y="0" dx="1.102396875" dy="1.399196875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.5692" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX43p4Y55p09D0T</text>
<text x="-18.0848" y="-10.1092" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.6492" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="CAPC220145_88N_KEM">
<smd name="1" x="-0.8028" y="0" dx="1.305596875" dy="1.449996875" layer="1"/>
<smd name="2" x="0.8028" y="0" dx="1.305596875" dy="1.449996875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.5946" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX51p4Y57p09D0T</text>
<text x="-18.0848" y="-10.1346" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.6746" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<polygon width="0.0254" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.0254" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.5946" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX51p4Y57p09D0T</text>
<text x="-18.0848" y="-10.1346" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.6746" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.0254" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.0254" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="CAPC220145_88N_KEM-M">
<smd name="1" x="-0.9044" y="0" dx="1.508796875" dy="1.500796875" layer="1"/>
<smd name="2" x="0.9044" y="0" dx="1.508796875" dy="1.500796875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.62" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX59p4Y59p09D0T</text>
<text x="-18.0848" y="-10.16" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.7" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.62" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX59p4Y59p09D0T</text>
<text x="-18.0848" y="-10.16" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.7" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="CAPC220145_88N_KEM-L">
<smd name="1" x="-0.7012" y="0" dx="1.102396875" dy="1.399196875" layer="1"/>
<smd name="2" x="0.7012" y="0" dx="1.102396875" dy="1.399196875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.5692" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX43p4Y55p09D0T</text>
<text x="-18.0848" y="-10.1092" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.6492" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.5692" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX43p4Y55p09D0T</text>
<text x="-18.0848" y="-10.1092" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.6492" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="CAPC220145_100N_KEM">
<smd name="1" x="-0.8028" y="0" dx="1.305596875" dy="1.449996875" layer="1"/>
<smd name="2" x="0.8028" y="0" dx="1.305596875" dy="1.449996875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.5946" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX51p4Y57p09D0T</text>
<text x="-18.0848" y="-10.1346" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.6746" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.0254" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.0254" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="CAPC220145_100N_KEM-M">
<smd name="1" x="-0.9044" y="0" dx="1.508796875" dy="1.500796875" layer="1"/>
<smd name="2" x="0.9044" y="0" dx="1.508796875" dy="1.500796875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.62" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX59p4Y59p09D0T</text>
<text x="-18.0848" y="-10.16" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.7" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
<package name="CAPC220145_100N_KEM-L">
<smd name="1" x="-0.7012" y="0" dx="1.102396875" dy="1.399196875" layer="1"/>
<smd name="2" x="0.7012" y="0" dx="1.102396875" dy="1.399196875" layer="1"/>
<wire x1="0" y1="0.7366" x2="3.6322" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="4.0132" y2="0.7366" width="0.1524" layer="48"/>
<wire x1="0" y1="-0.7366" x2="3.6322" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="4.0132" y2="-0.7366" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.6322" y2="2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.6322" y2="-2.0066" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.5052" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="0.7366" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="0.9906" x2="3.7592" y2="0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.5052" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.6322" y1="-0.7366" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="3.5052" y1="-0.9906" x2="3.7592" y2="-0.9906" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="0" x2="-1.0922" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="3.6576" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="2.3622" y2="3.2766" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="-1.0922" y1="3.2766" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="-1.3462" y1="3.4036" x2="-1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="3.4036" x2="1.3462" y2="3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="0" x2="0.1524" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="0.1524" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="3.2766" x2="1.0922" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.0922" y2="-3.6576" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-1.1176" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="2.3622" y2="-3.2766" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="0.1524" y1="-3.2766" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="-0.1016" y1="-3.1496" x2="-0.1016" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.1496" width="0.1524" layer="48"/>
<wire x1="1.0922" y1="-3.2766" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<wire x1="1.3462" y1="-3.1496" x2="1.3462" y2="-3.4036" width="0.1524" layer="48"/>
<text x="-18.0848" y="-7.5692" size="1.27" layer="48" ratio="6" rot="SR0">Default Padstyle: RX43p4Y55p09D0T</text>
<text x="-18.0848" y="-10.1092" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 1 Padstyle: OX60Y90D30P</text>
<text x="-18.0848" y="-12.6492" size="1.27" layer="48" ratio="6" rot="SR0">Alternate 2 Padstyle: OX90Y60D30P</text>
<text x="4.1402" y="-0.3048" size="0.635" layer="48" ratio="4" rot="SR0">0.057in/1.45mm</text>
<text x="-3.4544" y="3.7846" size="0.635" layer="48" ratio="4" rot="SR0">0.087in/2.2mm</text>
<text x="-2.8448" y="-4.4196" size="0.635" layer="48" ratio="4" rot="SR0">0.03in/0.75mm</text>
<wire x1="-0.3556" y1="-0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="0.7366" x2="-1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="-0.7366" x2="-0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="-0.7366" x2="1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="0.7366" x2="0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-0.3556" y1="-0.7366" x2="0.3556" y2="-0.7366" width="0.1524" layer="51"/>
<wire x1="1.0922" y1="-0.7366" x2="1.0922" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="0.3556" y1="0.7366" x2="-0.3556" y2="0.7366" width="0.1524" layer="51"/>
<wire x1="-1.0922" y1="0.7366" x2="-1.0922" y2="-0.7366" width="0.1524" layer="51"/>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.674196875"/>
<vertex x="0.0992" y="0.674196875"/>
<vertex x="0.0992" y="-0.674196875"/>
<vertex x="-0.0992" y="-0.674196875"/>
</polygon>
<polygon width="0.1524" layer="41">
<vertex x="-0.0992" y="0.724996875"/>
<vertex x="0.0992" y="0.724996875"/>
<vertex x="0.0992" y="-0.724996875"/>
<vertex x="-0.0992" y="-0.724996875"/>
</polygon>
<text x="-3.2766" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Name</text>
<text x="-1.7272" y="-0.635" size="1.27" layer="27" ratio="6" rot="SR0">&gt;Value</text>
</package>
</packages>
<symbols>
<symbol name="CAPH">
<pin name="2" x="7.62" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
<pin name="1" x="0" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
<wire x1="3.4798" y1="0" x2="3.4798" y2="1.905" width="0.2032" layer="94"/>
<wire x1="4.1148" y1="0" x2="4.1148" y2="1.905" width="0.2032" layer="94"/>
<text x="-5.1562" y="-9.3472" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-5.1562" y="-5.5372" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-4.0894" y="2.0828" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
<text x="-5.1562" y="-9.3472" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-5.1562" y="-5.5372" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-4.0894" y="2.0828" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
<text x="-5.1562" y="-9.3472" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-5.1562" y="-5.5372" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-4.0894" y="2.0828" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
<wire x1="3.4798" y1="-1.905" x2="3.4798" y2="0" width="0.2032" layer="94"/>
<wire x1="4.1148" y1="-1.905" x2="4.1148" y2="0" width="0.2032" layer="94"/>
<wire x1="4.1148" y1="0" x2="5.08" y2="0" width="0.2032" layer="94"/>
<wire x1="2.54" y1="0" x2="3.4798" y2="0" width="0.2032" layer="94"/>
<text x="-5.1562" y="-9.3472" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-5.1562" y="-5.5372" size="3.4798" layer="96" ratio="10" rot="SR0">&gt;Value</text>
<text x="-4.0894" y="2.0828" size="3.4798" layer="95" ratio="10" rot="SR0">&gt;Name</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="C0805C105K5RACTU" prefix="C">
<gates>
<gate name="A" symbol="CAPH" x="0" y="0" swaplevel="1"/>
</gates>
<devices>
<device name="" package="CAPC220145_140N_KEM">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C105K5RACTU" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
<device name="CAPC220145_140N_KEM-M" package="CAPC220145_140N_KEM-M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C105K5RACTU" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
<device name="CAPC220145_140N_KEM-L" package="CAPC220145_140N_KEM-L">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C105K5RACTU" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="C0805C103K5RACAUTO" prefix="C">
<gates>
<gate name="A" symbol="CAPH" x="0" y="0" swaplevel="1"/>
</gates>
<devices>
<device name="" package="CAPC220145_88N_KEM">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C103K5RACAUTO" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
<device name="CAPC220145_88N_KEM-M" package="CAPC220145_88N_KEM-M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C103K5RACAUTO" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
<device name="CAPC220145_88N_KEM-L" package="CAPC220145_88N_KEM-L">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C103K5RACAUTO" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="C0805C104K5RACTU" prefix="C">
<gates>
<gate name="A" symbol="CAPH" x="0" y="0" swaplevel="1"/>
</gates>
<devices>
<device name="" package="CAPC220145_88N_KEM">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C104K5RACTU" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
<device name="CAPC220145_88N_KEM-M" package="CAPC220145_88N_KEM-M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C104K5RACTU" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
<device name="CAPC220145_88N_KEM-L" package="CAPC220145_88N_KEM-L">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C104K5RACTU" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="C0805C224K5RACAUTO" prefix="C">
<gates>
<gate name="A" symbol="CAPH" x="0" y="0" swaplevel="1"/>
</gates>
<devices>
<device name="" package="CAPC220145_100N_KEM">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C224K5RACAUTO" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
<device name="CAPC220145_100N_KEM-M" package="CAPC220145_100N_KEM-M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C224K5RACAUTO" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
<device name="CAPC220145_100N_KEM-L" package="CAPC220145_100N_KEM-L">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="COPYRIGHT" value="Copyright (C) 2025 Ultra Librarian. All rights reserved." constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="C0805C224K5RACAUTO" constant="no"/>
<attribute name="MFR_NAME" value="Kemet" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="frames" urn="urn:adsk.eagle:library:229">
<description>&lt;b&gt;Frames for Sheet and Layout&lt;/b&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="FRAME_B_L" urn="urn:adsk.eagle:symbol:13883/1" library_version="1">
<frame x1="0" y1="0" x2="431.8" y2="279.4" columns="9" rows="6" layer="94" border-bottom="no"/>
</symbol>
<symbol name="DOCFIELD" urn="urn:adsk.eagle:symbol:13864/1" library_version="1">
<wire x1="0" y1="0" x2="71.12" y2="0" width="0.1016" layer="94"/>
<wire x1="101.6" y1="15.24" x2="87.63" y2="15.24" width="0.1016" layer="94"/>
<wire x1="0" y1="0" x2="0" y2="5.08" width="0.1016" layer="94"/>
<wire x1="0" y1="5.08" x2="71.12" y2="5.08" width="0.1016" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="15.24" width="0.1016" layer="94"/>
<wire x1="101.6" y1="15.24" x2="101.6" y2="5.08" width="0.1016" layer="94"/>
<wire x1="71.12" y1="5.08" x2="71.12" y2="0" width="0.1016" layer="94"/>
<wire x1="71.12" y1="5.08" x2="87.63" y2="5.08" width="0.1016" layer="94"/>
<wire x1="71.12" y1="0" x2="101.6" y2="0" width="0.1016" layer="94"/>
<wire x1="87.63" y1="15.24" x2="87.63" y2="5.08" width="0.1016" layer="94"/>
<wire x1="87.63" y1="15.24" x2="0" y2="15.24" width="0.1016" layer="94"/>
<wire x1="87.63" y1="5.08" x2="101.6" y2="5.08" width="0.1016" layer="94"/>
<wire x1="101.6" y1="5.08" x2="101.6" y2="0" width="0.1016" layer="94"/>
<wire x1="0" y1="15.24" x2="0" y2="22.86" width="0.1016" layer="94"/>
<wire x1="101.6" y1="35.56" x2="0" y2="35.56" width="0.1016" layer="94"/>
<wire x1="101.6" y1="35.56" x2="101.6" y2="22.86" width="0.1016" layer="94"/>
<wire x1="0" y1="22.86" x2="101.6" y2="22.86" width="0.1016" layer="94"/>
<wire x1="0" y1="22.86" x2="0" y2="35.56" width="0.1016" layer="94"/>
<wire x1="101.6" y1="22.86" x2="101.6" y2="15.24" width="0.1016" layer="94"/>
<text x="1.27" y="1.27" size="2.54" layer="94">Date:</text>
<text x="12.7" y="1.27" size="2.54" layer="94">&gt;LAST_DATE_TIME</text>
<text x="72.39" y="1.27" size="2.54" layer="94">Sheet:</text>
<text x="86.36" y="1.27" size="2.54" layer="94">&gt;SHEET</text>
<text x="88.9" y="11.43" size="2.54" layer="94">REV:</text>
<text x="1.27" y="19.05" size="2.54" layer="94">TITLE:</text>
<text x="1.27" y="11.43" size="2.54" layer="94">Document Number:</text>
<text x="17.78" y="19.05" size="2.54" layer="94">&gt;DRAWING_NAME</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="FRAME_B_L" urn="urn:adsk.eagle:component:13938/1" prefix="FRAME" uservalue="yes" library_version="1">
<description>&lt;b&gt;FRAME&lt;/b&gt; B Size , 11 x 17 INCH, Landscape&lt;p&gt;</description>
<gates>
<gate name="G$1" symbol="FRAME_B_L" x="0" y="0" addlevel="always"/>
<gate name="G$2" symbol="DOCFIELD" x="325.12" y="0" addlevel="always"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="U1" library="burakkbaltaa" deviceset="ADS131M08IPBS" device=""/>
<part name="R1" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="C1" library="C3225X7R1N106M250AC" deviceset="C0805C103K5RACAUTO" device="" value="10 nF"/>
<part name="C2" library="C3225X7R1N106M250AC" deviceset="C0805C103K5RACAUTO" device="" value="10 nF"/>
<part name="C3" library="C3225X7R1N106M250AC" deviceset="C0805C103K5RACAUTO" device="" value="10 nF"/>
<part name="C4" library="C3225X7R1N106M250AC" deviceset="C0805C103K5RACAUTO" device="" value="10 nF"/>
<part name="C5" library="C3225X7R1N106M250AC" deviceset="C0805C103K5RACAUTO" device="" value="10 nF"/>
<part name="C6" library="C3225X7R1N106M250AC" deviceset="C0805C103K5RACAUTO" device="" value="10 nF"/>
<part name="C7" library="C3225X7R1N106M250AC" deviceset="C0805C103K5RACAUTO" device="" value="10 nF"/>
<part name="C8" library="C3225X7R1N106M250AC" deviceset="C0805C103K5RACAUTO" device="" value="10 nF"/>
<part name="R2" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R3" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R4" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R5" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R6" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R7" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R8" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R9" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R10" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R11" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R12" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R13" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R14" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R15" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="R16" library="burakkbaltaa" deviceset="RC0402JR-071KL" device="RC0402N_YAG-M" value="1k"/>
<part name="C9" library="C3225X7R1N106M250AC" deviceset="C0805C105K5RACTU" device="" value="1 µF"/>
<part name="C10" library="C3225X7R1N106M250AC" deviceset="C0805C105K5RACTU" device="" value="1 µF"/>
<part name="C11" library="C3225X7R1N106M250AC" deviceset="C0805C224K5RACAUTO" device="" value="220 nF"/>
<part name="J3" library="burakkbaltaa" deviceset="282834-8" device=""/>
<part name="J4" library="burakkbaltaa" deviceset="282834-8" device=""/>
<part name="J2" library="burakkbaltaa" deviceset="282834-8" device=""/>
<part name="C13" library="C3225X7R1N106M250AC" deviceset="C0805C104K5RACTU" device="" value="100 nF"/>
<part name="R18" library="burakkbaltaa" deviceset="RC0402JR-07150RL" device="RC0402N_YAG-M" value="150"/>
<part name="C14" library="C3225X7R1N106M250AC" deviceset="C0805C105K5RACTU" device="" value="1 µF"/>
<part name="C15" library="C3225X7R1N106M250AC" deviceset="C0805C105K5RACTU" device="" value="1 µF"/>
<part name="LED1" library="burakkbaltaa" deviceset="LTST-C190KFKT" device=""/>
<part name="U3" library="burakkbaltaa" deviceset="XC6206P332MR-G" device="SOT-23_TOR-L"/>
<part name="FRAME1" library="frames" library_urn="urn:adsk.eagle:library:229" deviceset="FRAME_B_L" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="U1" gate="A" x="236.22" y="172.72" smashed="yes">
<attribute name="NAME" x="231.4956" y="174.2186" size="2.0828" layer="95" ratio="6" rot="SR0"/>
<attribute name="VALUE" x="230.8606" y="171.6786" size="2.0828" layer="96" ratio="6" rot="SR0"/>
<attribute name="VALUE" x="230.8606" y="171.6786" size="2.0828" layer="96" ratio="6" rot="SR0"/>
</instance>
<instance part="R1" gate="A" x="147.32" y="218.44" smashed="yes">
<attribute name="VALUE" x="144.7038" y="212.9028" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="209.7024" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="220.5228" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="C1" gate="A" x="172.72" y="218.44" smashed="yes" rot="R270">
<attribute name="VALUE" x="163.3728" y="223.5962" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="VALUE" x="167.1828" y="223.5962" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="NAME" x="174.8028" y="222.5294" size="3.4798" layer="95" ratio="10" rot="SR270"/>
</instance>
<instance part="C2" gate="A" x="172.72" y="208.28" smashed="yes" rot="R270">
<attribute name="VALUE" x="163.3728" y="213.4362" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="VALUE" x="167.1828" y="213.4362" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="NAME" x="174.8028" y="212.3694" size="3.4798" layer="95" ratio="10" rot="SR270"/>
</instance>
<instance part="C3" gate="A" x="172.72" y="198.12" smashed="yes" rot="R270">
<attribute name="VALUE" x="163.3728" y="203.2762" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="VALUE" x="167.1828" y="203.2762" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="NAME" x="174.8028" y="202.2094" size="3.4798" layer="95" ratio="10" rot="SR270"/>
</instance>
<instance part="C4" gate="A" x="172.72" y="187.96" smashed="yes" rot="R270">
<attribute name="VALUE" x="163.3728" y="193.1162" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="VALUE" x="167.1828" y="193.1162" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="NAME" x="174.8028" y="192.0494" size="3.4798" layer="95" ratio="10" rot="SR270"/>
</instance>
<instance part="C5" gate="A" x="172.72" y="177.8" smashed="yes" rot="R270">
<attribute name="VALUE" x="163.3728" y="182.9562" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="VALUE" x="167.1828" y="182.9562" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="NAME" x="174.8028" y="181.8894" size="3.4798" layer="95" ratio="10" rot="SR270"/>
</instance>
<instance part="C6" gate="A" x="172.72" y="167.64" smashed="yes" rot="R270">
<attribute name="VALUE" x="163.3728" y="172.7962" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="VALUE" x="167.1828" y="172.7962" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="NAME" x="174.8028" y="171.7294" size="3.4798" layer="95" ratio="10" rot="SR270"/>
</instance>
<instance part="C7" gate="A" x="172.72" y="157.48" smashed="yes" rot="R270">
<attribute name="VALUE" x="163.3728" y="162.6362" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="VALUE" x="167.1828" y="162.6362" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="NAME" x="174.8028" y="161.5694" size="3.4798" layer="95" ratio="10" rot="SR270"/>
</instance>
<instance part="C8" gate="A" x="172.72" y="147.32" smashed="yes" rot="R270">
<attribute name="VALUE" x="163.3728" y="152.4762" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="VALUE" x="167.1828" y="152.4762" size="3.4798" layer="96" ratio="10" rot="SR270"/>
<attribute name="NAME" x="174.8028" y="151.4094" size="3.4798" layer="95" ratio="10" rot="SR270"/>
</instance>
<instance part="R2" gate="A" x="147.32" y="210.82" smashed="yes">
<attribute name="VALUE" x="144.7038" y="205.2828" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="202.0824" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="212.9028" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R3" gate="A" x="147.32" y="208.28" smashed="yes">
<attribute name="VALUE" x="144.7038" y="202.7428" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="199.5424" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="210.3628" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R4" gate="A" x="147.32" y="200.66" smashed="yes">
<attribute name="VALUE" x="144.7038" y="195.1228" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="191.9224" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="202.7428" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R5" gate="A" x="147.32" y="198.12" smashed="yes">
<attribute name="VALUE" x="144.7038" y="192.5828" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="189.3824" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="200.2028" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R6" gate="A" x="147.32" y="190.5" smashed="yes">
<attribute name="VALUE" x="144.7038" y="184.9628" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="181.7624" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="192.5828" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R7" gate="A" x="147.32" y="187.96" smashed="yes">
<attribute name="VALUE" x="144.7038" y="182.4228" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="179.2224" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="190.0428" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R8" gate="A" x="147.32" y="180.34" smashed="yes">
<attribute name="VALUE" x="144.7038" y="174.8028" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="171.6024" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="182.4228" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R9" gate="A" x="147.32" y="177.8" smashed="yes">
<attribute name="VALUE" x="144.7038" y="172.2628" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="169.0624" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="179.8828" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R10" gate="A" x="147.32" y="170.18" smashed="yes">
<attribute name="VALUE" x="144.7038" y="164.6428" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="161.4424" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="172.2628" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R11" gate="A" x="147.32" y="167.64" smashed="yes">
<attribute name="VALUE" x="144.7038" y="162.1028" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="158.9024" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="169.7228" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R12" gate="A" x="147.32" y="160.02" smashed="yes">
<attribute name="VALUE" x="144.7038" y="154.4828" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="151.2824" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="162.1028" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R13" gate="A" x="147.32" y="157.48" smashed="yes">
<attribute name="VALUE" x="144.7038" y="151.9428" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="148.7424" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="159.5628" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R14" gate="A" x="147.32" y="149.86" smashed="yes">
<attribute name="VALUE" x="144.7038" y="144.3228" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="141.1224" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="151.9428" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R15" gate="A" x="147.32" y="147.32" smashed="yes">
<attribute name="VALUE" x="144.7038" y="141.7828" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="138.5824" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="149.4028" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="R16" gate="A" x="147.32" y="139.7" smashed="yes">
<attribute name="VALUE" x="144.7038" y="134.1628" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="VALUE" x="144.7038" y="130.9624" size="3.4798" layer="96" ratio="10" rot="SR0"/>
<attribute name="NAME" x="145.1356" y="141.7828" size="3.4798" layer="95" ratio="10" rot="SR0"/>
</instance>
<instance part="C9" gate="A" x="195.58" y="121.92" smashed="yes">
<attribute name="NAME" x="195.58" y="125.73" size="1.778" layer="95"/>
<attribute name="VALUE" x="195.58" y="116.832" size="1.778" layer="96"/>
</instance>
<instance part="C10" gate="A" x="276.86" y="127" smashed="yes">
<attribute name="NAME" x="276.86" y="130.81" size="1.778" layer="95"/>
<attribute name="VALUE" x="276.86" y="121.912" size="1.778" layer="96"/>
</instance>
<instance part="C11" gate="A" x="276.86" y="210.82" smashed="yes">
<attribute name="NAME" x="276.86" y="214.63" size="1.778" layer="95"/>
<attribute name="VALUE" x="276.86" y="205.732" size="1.778" layer="96"/>
</instance>
<instance part="J3" gate="A" x="111.76" y="185.42" smashed="yes" rot="R180">
<attribute name="NAME" x="107.5944" y="180.1114" size="2.0828" layer="95" ratio="6" rot="SR180"/>
<attribute name="VALUE" x="106.9594" y="196.6214" size="2.0828" layer="96" ratio="6" rot="SR180"/>
</instance>
<instance part="J4" gate="A" x="111.76" y="162.56" smashed="yes" rot="R180">
<attribute name="NAME" x="107.5944" y="157.2514" size="2.0828" layer="95" ratio="6" rot="SR180"/>
<attribute name="VALUE" x="106.9594" y="173.7614" size="2.0828" layer="96" ratio="6" rot="SR180"/>
</instance>
<instance part="J2" gate="A" x="312.42" y="170.18" smashed="yes">
<attribute name="NAME" x="316.5856" y="175.4886" size="2.0828" layer="95" ratio="6" rot="SR0"/>
<attribute name="VALUE" x="317.2206" y="158.9786" size="2.0828" layer="96" ratio="6" rot="SR0"/>
</instance>
<instance part="C13" gate="A" x="172.72" y="127" smashed="yes">
<attribute name="NAME" x="172.72" y="130.81" size="1.778" layer="95"/>
<attribute name="VALUE" x="172.72" y="121.912" size="1.778" layer="96"/>
</instance>
<instance part="R18" gate="A" x="213.36" y="86.36" smashed="yes">
<attribute name="VALUE" x="210.7438" y="80.8228" size="3.4798" layer="96" ratio="10" rot="SR0"/>
</instance>
<instance part="C14" gate="A" x="302.26" y="99.06" smashed="yes">
<attribute name="NAME" x="302.26" y="102.87" size="1.778" layer="95"/>
<attribute name="VALUE" x="302.26" y="93.972" size="1.778" layer="96"/>
</instance>
<instance part="C15" gate="A" x="241.3" y="104.14" smashed="yes" rot="R270">
<attribute name="NAME" x="245.11" y="106.68" size="1.778" layer="95" rot="R270"/>
<attribute name="VALUE" x="236.212" y="104.14" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="LED1" gate="A" x="231.14" y="99.06" smashed="yes" rot="R270"/>
<instance part="U3" gate="A" x="297.18" y="99.06" smashed="yes" rot="R180">
<attribute name="NAME" x="284.1244" y="95.0214" size="2.0828" layer="95" ratio="6" rot="SR180"/>
</instance>
<instance part="FRAME1" gate="G$1" x="0" y="0" smashed="yes"/>
<instance part="FRAME1" gate="G$2" x="325.12" y="0" smashed="yes">
<attribute name="LAST_DATE_TIME" x="337.82" y="1.27" size="2.54" layer="94"/>
<attribute name="SHEET" x="411.48" y="1.27" size="2.54" layer="94"/>
<attribute name="DRAWING_NAME" x="342.9" y="19.05" size="2.54" layer="94"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN0P"/>
<wire x1="210.82" y1="210.82" x2="203.2" y2="210.82" width="0.1524" layer="91"/>
<wire x1="203.2" y1="210.82" x2="203.2" y2="218.44" width="0.1524" layer="91"/>
<pinref part="C1" gate="A" pin="1"/>
<wire x1="203.2" y1="218.44" x2="172.72" y2="218.44" width="0.1524" layer="91"/>
<pinref part="R1" gate="A" pin="1"/>
<wire x1="172.72" y1="218.44" x2="160.02" y2="218.44" width="0.1524" layer="91"/>
<junction x="172.72" y="218.44"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN0N"/>
<wire x1="210.82" y1="208.28" x2="200.66" y2="208.28" width="0.1524" layer="91"/>
<wire x1="200.66" y1="208.28" x2="200.66" y2="215.9" width="0.1524" layer="91"/>
<wire x1="200.66" y1="215.9" x2="175.26" y2="215.9" width="0.1524" layer="91"/>
<wire x1="175.26" y1="215.9" x2="175.26" y2="210.82" width="0.1524" layer="91"/>
<pinref part="C1" gate="A" pin="2"/>
<wire x1="175.26" y1="210.82" x2="172.72" y2="210.82" width="0.1524" layer="91"/>
<pinref part="R2" gate="A" pin="1"/>
<wire x1="172.72" y1="210.82" x2="160.02" y2="210.82" width="0.1524" layer="91"/>
<junction x="172.72" y="210.82"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN1N"/>
<wire x1="210.82" y1="203.2" x2="175.26" y2="203.2" width="0.1524" layer="91"/>
<wire x1="175.26" y1="203.2" x2="175.26" y2="208.28" width="0.1524" layer="91"/>
<pinref part="C2" gate="A" pin="1"/>
<wire x1="175.26" y1="208.28" x2="172.72" y2="208.28" width="0.1524" layer="91"/>
<pinref part="R3" gate="A" pin="1"/>
<wire x1="172.72" y1="208.28" x2="160.02" y2="208.28" width="0.1524" layer="91"/>
<junction x="172.72" y="208.28"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN1P"/>
<pinref part="C2" gate="A" pin="2"/>
<wire x1="210.82" y1="200.66" x2="172.72" y2="200.66" width="0.1524" layer="91"/>
<pinref part="R4" gate="A" pin="1"/>
<wire x1="172.72" y1="200.66" x2="160.02" y2="200.66" width="0.1524" layer="91"/>
<junction x="172.72" y="200.66"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN2P"/>
<wire x1="210.82" y1="195.58" x2="175.26" y2="195.58" width="0.1524" layer="91"/>
<wire x1="175.26" y1="195.58" x2="175.26" y2="198.12" width="0.1524" layer="91"/>
<pinref part="C3" gate="A" pin="1"/>
<wire x1="175.26" y1="198.12" x2="172.72" y2="198.12" width="0.1524" layer="91"/>
<pinref part="R5" gate="A" pin="1"/>
<wire x1="172.72" y1="198.12" x2="160.02" y2="198.12" width="0.1524" layer="91"/>
<junction x="172.72" y="198.12"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN2N"/>
<wire x1="210.82" y1="193.04" x2="175.26" y2="193.04" width="0.1524" layer="91"/>
<wire x1="175.26" y1="193.04" x2="175.26" y2="190.5" width="0.1524" layer="91"/>
<pinref part="C3" gate="A" pin="2"/>
<wire x1="175.26" y1="190.5" x2="172.72" y2="190.5" width="0.1524" layer="91"/>
<pinref part="R6" gate="A" pin="1"/>
<wire x1="172.72" y1="190.5" x2="160.02" y2="190.5" width="0.1524" layer="91"/>
<junction x="172.72" y="190.5"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN3N"/>
<pinref part="C4" gate="A" pin="1"/>
<wire x1="210.82" y1="187.96" x2="172.72" y2="187.96" width="0.1524" layer="91"/>
<pinref part="R7" gate="A" pin="1"/>
<wire x1="172.72" y1="187.96" x2="160.02" y2="187.96" width="0.1524" layer="91"/>
<junction x="172.72" y="187.96"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN3P"/>
<wire x1="210.82" y1="185.42" x2="177.8" y2="185.42" width="0.1524" layer="91"/>
<wire x1="177.8" y1="185.42" x2="177.8" y2="180.34" width="0.1524" layer="91"/>
<pinref part="C4" gate="A" pin="2"/>
<wire x1="177.8" y1="180.34" x2="172.72" y2="180.34" width="0.1524" layer="91"/>
<pinref part="R8" gate="A" pin="1"/>
<wire x1="172.72" y1="180.34" x2="160.02" y2="180.34" width="0.1524" layer="91"/>
<junction x="172.72" y="180.34"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN4P"/>
<wire x1="210.82" y1="180.34" x2="180.34" y2="180.34" width="0.1524" layer="91"/>
<wire x1="180.34" y1="180.34" x2="180.34" y2="177.8" width="0.1524" layer="91"/>
<pinref part="C5" gate="A" pin="1"/>
<wire x1="180.34" y1="177.8" x2="172.72" y2="177.8" width="0.1524" layer="91"/>
<pinref part="R9" gate="A" pin="1"/>
<wire x1="172.72" y1="177.8" x2="160.02" y2="177.8" width="0.1524" layer="91"/>
<junction x="172.72" y="177.8"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN4N"/>
<wire x1="210.82" y1="177.8" x2="182.88" y2="177.8" width="0.1524" layer="91"/>
<wire x1="182.88" y1="177.8" x2="182.88" y2="175.26" width="0.1524" layer="91"/>
<wire x1="182.88" y1="175.26" x2="175.26" y2="175.26" width="0.1524" layer="91"/>
<wire x1="175.26" y1="175.26" x2="175.26" y2="170.18" width="0.1524" layer="91"/>
<pinref part="C5" gate="A" pin="2"/>
<wire x1="175.26" y1="170.18" x2="172.72" y2="170.18" width="0.1524" layer="91"/>
<pinref part="R10" gate="A" pin="1"/>
<wire x1="172.72" y1="170.18" x2="160.02" y2="170.18" width="0.1524" layer="91"/>
<junction x="172.72" y="170.18"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN5N"/>
<wire x1="210.82" y1="172.72" x2="187.96" y2="172.72" width="0.1524" layer="91"/>
<wire x1="187.96" y1="172.72" x2="187.96" y2="167.64" width="0.1524" layer="91"/>
<pinref part="C6" gate="A" pin="1"/>
<wire x1="187.96" y1="167.64" x2="172.72" y2="167.64" width="0.1524" layer="91"/>
<pinref part="R11" gate="A" pin="1"/>
<wire x1="172.72" y1="167.64" x2="160.02" y2="167.64" width="0.1524" layer="91"/>
<junction x="172.72" y="167.64"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN5P"/>
<wire x1="210.82" y1="170.18" x2="190.5" y2="170.18" width="0.1524" layer="91"/>
<pinref part="C6" gate="A" pin="2"/>
<wire x1="190.5" y1="170.18" x2="190.5" y2="160.02" width="0.1524" layer="91"/>
<wire x1="190.5" y1="160.02" x2="172.72" y2="160.02" width="0.1524" layer="91"/>
<pinref part="R12" gate="A" pin="1"/>
<wire x1="172.72" y1="160.02" x2="160.02" y2="160.02" width="0.1524" layer="91"/>
<junction x="172.72" y="160.02"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN6P"/>
<wire x1="210.82" y1="165.1" x2="193.04" y2="165.1" width="0.1524" layer="91"/>
<wire x1="193.04" y1="165.1" x2="193.04" y2="157.48" width="0.1524" layer="91"/>
<pinref part="C7" gate="A" pin="1"/>
<wire x1="193.04" y1="157.48" x2="172.72" y2="157.48" width="0.1524" layer="91"/>
<pinref part="R13" gate="A" pin="1"/>
<wire x1="172.72" y1="157.48" x2="160.02" y2="157.48" width="0.1524" layer="91"/>
<junction x="172.72" y="157.48"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN6N"/>
<wire x1="210.82" y1="162.56" x2="195.58" y2="162.56" width="0.1524" layer="91"/>
<wire x1="195.58" y1="162.56" x2="195.58" y2="149.86" width="0.1524" layer="91"/>
<pinref part="C7" gate="A" pin="2"/>
<wire x1="195.58" y1="149.86" x2="172.72" y2="149.86" width="0.1524" layer="91"/>
<pinref part="R14" gate="A" pin="1"/>
<wire x1="172.72" y1="149.86" x2="160.02" y2="149.86" width="0.1524" layer="91"/>
<junction x="172.72" y="149.86"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN7N"/>
<wire x1="210.82" y1="157.48" x2="198.12" y2="157.48" width="0.1524" layer="91"/>
<wire x1="198.12" y1="157.48" x2="198.12" y2="147.32" width="0.1524" layer="91"/>
<pinref part="C8" gate="A" pin="1"/>
<wire x1="198.12" y1="147.32" x2="172.72" y2="147.32" width="0.1524" layer="91"/>
<pinref part="R15" gate="A" pin="1"/>
<wire x1="172.72" y1="147.32" x2="160.02" y2="147.32" width="0.1524" layer="91"/>
<junction x="172.72" y="147.32"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<pinref part="U1" gate="A" pin="AIN7P"/>
<wire x1="210.82" y1="154.94" x2="200.66" y2="154.94" width="0.1524" layer="91"/>
<wire x1="200.66" y1="154.94" x2="200.66" y2="144.78" width="0.1524" layer="91"/>
<pinref part="R16" gate="A" pin="1"/>
<pinref part="C8" gate="A" pin="2"/>
<wire x1="172.72" y1="139.7" x2="160.02" y2="139.7" width="0.1524" layer="91"/>
<wire x1="172.72" y1="139.7" x2="180.34" y2="139.7" width="0.1524" layer="91"/>
<junction x="172.72" y="139.7"/>
<wire x1="180.34" y1="139.7" x2="180.34" y2="144.78" width="0.1524" layer="91"/>
<wire x1="180.34" y1="144.78" x2="200.66" y2="144.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="U1" gate="A" pin="AGND"/>
<wire x1="210.82" y1="132.08" x2="208.28" y2="132.08" width="0.1524" layer="91"/>
<label x="205.74" y="132.08" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="U1" gate="A" pin="DGND"/>
<wire x1="261.62" y1="132.08" x2="264.16" y2="132.08" width="0.1524" layer="91"/>
<pinref part="C10" gate="A" pin="1"/>
<wire x1="276.86" y1="127" x2="264.16" y2="127" width="0.1524" layer="91"/>
<wire x1="264.16" y1="127" x2="264.16" y2="132.08" width="0.1524" layer="91"/>
<label x="266.7" y="132.08" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="U1" gate="A" pin="AGND_REFGND"/>
<label x="205.74" y="137.16" size="1.778" layer="95"/>
<wire x1="210.82" y1="137.16" x2="205.74" y2="137.16" width="0.1524" layer="91"/>
<wire x1="205.74" y1="137.16" x2="205.74" y2="121.92" width="0.1524" layer="91"/>
<pinref part="C9" gate="A" pin="2"/>
<wire x1="205.74" y1="121.92" x2="203.2" y2="121.92" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="C11" gate="A" pin="2"/>
<label x="289.56" y="210.82" size="1.778" layer="95"/>
<wire x1="284.48" y1="210.82" x2="289.56" y2="210.82" width="0.1524" layer="91"/>
</segment>
<segment>
<label x="312.42" y="147.32" size="1.778" layer="95"/>
<pinref part="J2" gate="A" pin="8"/>
<wire x1="312.42" y1="152.4" x2="312.42" y2="149.86" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="C13" gate="A" pin="1"/>
<wire x1="172.72" y1="127" x2="162.56" y2="127" width="0.1524" layer="91"/>
<label x="162.56" y="129.54" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="C14" gate="A" pin="1"/>
<wire x1="302.26" y1="99.06" x2="302.26" y2="83.82" width="0.1524" layer="91"/>
<label x="304.8" y="83.82" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="C15" gate="A" pin="2"/>
<wire x1="241.3" y1="96.52" x2="241.3" y2="91.44" width="0.1524" layer="91"/>
<label x="241.3" y="88.9" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="R18" gate="A" pin="2"/>
<wire x1="213.36" y1="86.36" x2="208.28" y2="86.36" width="0.1524" layer="91"/>
<label x="205.74" y="86.36" size="1.778" layer="95"/>
</segment>
<segment>
<pinref part="U3" gate="A" pin="VSS"/>
<wire x1="276.86" y1="119.38" x2="284.48" y2="119.38" width="0.1524" layer="91"/>
<label x="284.48" y="119.38" size="1.778" layer="95"/>
</segment>
</net>
<net name="N$17" class="0">
<segment>
<pinref part="U1" gate="A" pin="LDOCAP"/>
<pinref part="C11" gate="A" pin="1"/>
<wire x1="261.62" y1="205.74" x2="276.86" y2="205.74" width="0.1524" layer="91"/>
<wire x1="276.86" y1="205.74" x2="276.86" y2="210.82" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$20" class="0">
<segment>
<pinref part="R1" gate="A" pin="2"/>
<wire x1="147.32" y1="218.44" x2="129.54" y2="218.44" width="0.1524" layer="91"/>
<wire x1="129.54" y1="218.44" x2="129.54" y2="203.2" width="0.1524" layer="91"/>
<pinref part="J3" gate="A" pin="8"/>
<wire x1="129.54" y1="203.2" x2="111.76" y2="203.2" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$21" class="0">
<segment>
<pinref part="R2" gate="A" pin="2"/>
<wire x1="147.32" y1="210.82" x2="132.08" y2="210.82" width="0.1524" layer="91"/>
<wire x1="132.08" y1="210.82" x2="132.08" y2="200.66" width="0.1524" layer="91"/>
<pinref part="J3" gate="A" pin="7"/>
<wire x1="132.08" y1="200.66" x2="111.76" y2="200.66" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$22" class="0">
<segment>
<pinref part="R3" gate="A" pin="2"/>
<wire x1="147.32" y1="208.28" x2="134.62" y2="208.28" width="0.1524" layer="91"/>
<wire x1="134.62" y1="208.28" x2="134.62" y2="198.12" width="0.1524" layer="91"/>
<pinref part="J3" gate="A" pin="6"/>
<wire x1="134.62" y1="198.12" x2="111.76" y2="198.12" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$23" class="0">
<segment>
<pinref part="R4" gate="A" pin="2"/>
<wire x1="147.32" y1="200.66" x2="137.16" y2="200.66" width="0.1524" layer="91"/>
<wire x1="137.16" y1="200.66" x2="137.16" y2="195.58" width="0.1524" layer="91"/>
<pinref part="J3" gate="A" pin="5"/>
<wire x1="137.16" y1="195.58" x2="111.76" y2="195.58" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$24" class="0">
<segment>
<pinref part="R5" gate="A" pin="2"/>
<wire x1="147.32" y1="198.12" x2="139.7" y2="198.12" width="0.1524" layer="91"/>
<wire x1="139.7" y1="198.12" x2="139.7" y2="193.04" width="0.1524" layer="91"/>
<pinref part="J3" gate="A" pin="4"/>
<wire x1="139.7" y1="193.04" x2="111.76" y2="193.04" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$25" class="0">
<segment>
<pinref part="R6" gate="A" pin="2"/>
<pinref part="J3" gate="A" pin="3"/>
<wire x1="147.32" y1="190.5" x2="111.76" y2="190.5" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$26" class="0">
<segment>
<pinref part="R7" gate="A" pin="2"/>
<pinref part="J3" gate="A" pin="2"/>
<wire x1="147.32" y1="187.96" x2="111.76" y2="187.96" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$27" class="0">
<segment>
<pinref part="R8" gate="A" pin="2"/>
<wire x1="147.32" y1="180.34" x2="134.62" y2="180.34" width="0.1524" layer="91"/>
<pinref part="J3" gate="A" pin="1"/>
<wire x1="134.62" y1="180.34" x2="134.62" y2="185.42" width="0.1524" layer="91"/>
<wire x1="134.62" y1="185.42" x2="111.76" y2="185.42" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$28" class="0">
<segment>
<pinref part="R9" gate="A" pin="2"/>
<wire x1="147.32" y1="177.8" x2="132.08" y2="177.8" width="0.1524" layer="91"/>
<wire x1="132.08" y1="177.8" x2="132.08" y2="180.34" width="0.1524" layer="91"/>
<pinref part="J4" gate="A" pin="8"/>
<wire x1="132.08" y1="180.34" x2="111.76" y2="180.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$29" class="0">
<segment>
<pinref part="R10" gate="A" pin="2"/>
<wire x1="147.32" y1="170.18" x2="144.78" y2="170.18" width="0.1524" layer="91"/>
<wire x1="144.78" y1="170.18" x2="144.78" y2="175.26" width="0.1524" layer="91"/>
<wire x1="144.78" y1="175.26" x2="129.54" y2="175.26" width="0.1524" layer="91"/>
<wire x1="129.54" y1="175.26" x2="129.54" y2="177.8" width="0.1524" layer="91"/>
<pinref part="J4" gate="A" pin="7"/>
<wire x1="129.54" y1="177.8" x2="111.76" y2="177.8" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$30" class="0">
<segment>
<pinref part="R11" gate="A" pin="2"/>
<wire x1="147.32" y1="167.64" x2="142.24" y2="167.64" width="0.1524" layer="91"/>
<wire x1="142.24" y1="167.64" x2="142.24" y2="172.72" width="0.1524" layer="91"/>
<wire x1="142.24" y1="172.72" x2="127" y2="172.72" width="0.1524" layer="91"/>
<wire x1="127" y1="172.72" x2="127" y2="175.26" width="0.1524" layer="91"/>
<pinref part="J4" gate="A" pin="6"/>
<wire x1="127" y1="175.26" x2="111.76" y2="175.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$31" class="0">
<segment>
<pinref part="R12" gate="A" pin="2"/>
<wire x1="147.32" y1="160.02" x2="147.32" y2="162.56" width="0.1524" layer="91"/>
<wire x1="147.32" y1="162.56" x2="137.16" y2="162.56" width="0.1524" layer="91"/>
<wire x1="137.16" y1="162.56" x2="137.16" y2="170.18" width="0.1524" layer="91"/>
<wire x1="137.16" y1="170.18" x2="124.46" y2="170.18" width="0.1524" layer="91"/>
<wire x1="124.46" y1="170.18" x2="124.46" y2="172.72" width="0.1524" layer="91"/>
<pinref part="J4" gate="A" pin="5"/>
<wire x1="124.46" y1="172.72" x2="111.76" y2="172.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$32" class="0">
<segment>
<pinref part="R13" gate="A" pin="2"/>
<wire x1="147.32" y1="157.48" x2="134.62" y2="157.48" width="0.1524" layer="91"/>
<wire x1="134.62" y1="157.48" x2="134.62" y2="167.64" width="0.1524" layer="91"/>
<wire x1="134.62" y1="167.64" x2="121.92" y2="167.64" width="0.1524" layer="91"/>
<wire x1="121.92" y1="167.64" x2="121.92" y2="170.18" width="0.1524" layer="91"/>
<pinref part="J4" gate="A" pin="4"/>
<wire x1="121.92" y1="170.18" x2="111.76" y2="170.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$33" class="0">
<segment>
<pinref part="R14" gate="A" pin="2"/>
<wire x1="147.32" y1="149.86" x2="119.38" y2="149.86" width="0.1524" layer="91"/>
<pinref part="J4" gate="A" pin="3"/>
<wire x1="119.38" y1="149.86" x2="119.38" y2="167.64" width="0.1524" layer="91"/>
<wire x1="119.38" y1="167.64" x2="111.76" y2="167.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$34" class="0">
<segment>
<pinref part="R15" gate="A" pin="2"/>
<wire x1="147.32" y1="147.32" x2="116.84" y2="147.32" width="0.1524" layer="91"/>
<wire x1="116.84" y1="147.32" x2="116.84" y2="165.1" width="0.1524" layer="91"/>
<pinref part="J4" gate="A" pin="2"/>
<wire x1="116.84" y1="165.1" x2="111.76" y2="165.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$35" class="0">
<segment>
<pinref part="R16" gate="A" pin="2"/>
<wire x1="147.32" y1="139.7" x2="114.3" y2="139.7" width="0.1524" layer="91"/>
<wire x1="114.3" y1="139.7" x2="114.3" y2="162.56" width="0.1524" layer="91"/>
<pinref part="J4" gate="A" pin="1"/>
<wire x1="114.3" y1="162.56" x2="111.76" y2="162.56" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$37" class="0">
<segment>
<pinref part="U1" gate="A" pin="CS_N"/>
<wire x1="261.62" y1="157.48" x2="312.42" y2="157.48" width="0.1524" layer="91"/>
<pinref part="J2" gate="A" pin="6"/>
</segment>
</net>
<net name="N$39" class="0">
<segment>
<pinref part="U1" gate="A" pin="DOUT"/>
<wire x1="261.62" y1="172.72" x2="297.18" y2="172.72" width="0.1524" layer="91"/>
<wire x1="297.18" y1="172.72" x2="297.18" y2="165.1" width="0.1524" layer="91"/>
<pinref part="J2" gate="A" pin="3"/>
<wire x1="297.18" y1="165.1" x2="312.42" y2="165.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$40" class="0">
<segment>
<pinref part="U1" gate="A" pin="DIN"/>
<wire x1="261.62" y1="177.8" x2="299.72" y2="177.8" width="0.1524" layer="91"/>
<wire x1="299.72" y1="177.8" x2="299.72" y2="167.64" width="0.1524" layer="91"/>
<pinref part="J2" gate="A" pin="2"/>
<wire x1="299.72" y1="167.64" x2="312.42" y2="167.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$41" class="0">
<segment>
<pinref part="U1" gate="A" pin="REFIN"/>
<wire x1="210.82" y1="142.24" x2="182.88" y2="142.24" width="0.1524" layer="91"/>
<wire x1="182.88" y1="142.24" x2="182.88" y2="127" width="0.1524" layer="91"/>
<pinref part="C13" gate="A" pin="2"/>
<wire x1="182.88" y1="127" x2="180.34" y2="127" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$52" class="0">
<segment>
<pinref part="J2" gate="A" pin="7"/>
<wire x1="312.42" y1="154.94" x2="309.88" y2="154.94" width="0.1524" layer="91"/>
<wire x1="309.88" y1="154.94" x2="309.88" y2="109.22" width="0.1524" layer="91"/>
<pinref part="C14" gate="A" pin="2"/>
<wire x1="309.88" y1="109.22" x2="309.88" y2="99.06" width="0.1524" layer="91"/>
<wire x1="297.18" y1="109.22" x2="309.88" y2="109.22" width="0.1524" layer="91"/>
<junction x="309.88" y="109.22"/>
<pinref part="U3" gate="A" pin="VIN"/>
<wire x1="297.18" y1="99.06" x2="297.18" y2="109.22" width="0.1524" layer="91"/>
</segment>
</net>
<net name="POWER" class="0">
<segment>
<pinref part="U1" gate="A" pin="AVDD_2"/>
<wire x1="210.82" y1="147.32" x2="203.2" y2="147.32" width="0.1524" layer="91"/>
<wire x1="203.2" y1="147.32" x2="203.2" y2="137.16" width="0.1524" layer="91"/>
<wire x1="203.2" y1="137.16" x2="190.5" y2="137.16" width="0.1524" layer="91"/>
<wire x1="190.5" y1="137.16" x2="190.5" y2="121.92" width="0.1524" layer="91"/>
<pinref part="C9" gate="A" pin="1"/>
<wire x1="190.5" y1="121.92" x2="195.58" y2="121.92" width="0.1524" layer="91"/>
<wire x1="195.58" y1="121.92" x2="195.58" y2="104.14" width="0.1524" layer="91"/>
<junction x="195.58" y="121.92"/>
<label x="195.58" y="106.68" size="1.778" layer="95"/>
<pinref part="C15" gate="A" pin="1"/>
<wire x1="241.3" y1="104.14" x2="231.14" y2="104.14" width="0.1524" layer="91"/>
<wire x1="231.14" y1="104.14" x2="195.58" y2="104.14" width="0.1524" layer="91"/>
<wire x1="254" y1="106.68" x2="241.3" y2="106.68" width="0.1524" layer="91"/>
<wire x1="241.3" y1="106.68" x2="241.3" y2="104.14" width="0.1524" layer="91"/>
<junction x="241.3" y="104.14"/>
<wire x1="241.3" y1="106.68" x2="241.3" y2="121.92" width="0.1524" layer="91"/>
<junction x="241.3" y="106.68"/>
<wire x1="241.3" y1="121.92" x2="271.78" y2="121.92" width="0.1524" layer="91"/>
<wire x1="287.02" y1="142.24" x2="287.02" y2="127" width="0.1524" layer="91"/>
<pinref part="C10" gate="A" pin="2"/>
<wire x1="287.02" y1="127" x2="284.48" y2="127" width="0.1524" layer="91"/>
<pinref part="U1" gate="A" pin="IOVDD"/>
<wire x1="287.02" y1="142.24" x2="271.78" y2="142.24" width="0.1524" layer="91"/>
<wire x1="271.78" y1="142.24" x2="269.24" y2="142.24" width="0.1524" layer="91"/>
<wire x1="269.24" y1="142.24" x2="261.62" y2="142.24" width="0.1524" layer="91"/>
<wire x1="261.62" y1="142.24" x2="261.62" y2="139.7" width="0.1524" layer="91"/>
<wire x1="271.78" y1="121.92" x2="271.78" y2="142.24" width="0.1524" layer="91"/>
<junction x="271.78" y="142.24"/>
<wire x1="231.14" y1="104.14" x2="231.14" y2="99.06" width="0.1524" layer="91"/>
<junction x="231.14" y="104.14"/>
<pinref part="LED1" gate="A" pin="ANODE"/>
<pinref part="U3" gate="A" pin="VOUT"/>
<wire x1="256.54" y1="99.06" x2="254" y2="99.06" width="0.1524" layer="91"/>
<wire x1="254" y1="99.06" x2="254" y2="106.68" width="0.1524" layer="91"/>
<pinref part="U1" gate="A" pin="SYNC_RESET_N"/>
<wire x1="261.62" y1="152.4" x2="269.24" y2="152.4" width="0.1524" layer="91"/>
<wire x1="269.24" y1="152.4" x2="269.24" y2="142.24" width="0.1524" layer="91"/>
<junction x="269.24" y="142.24"/>
</segment>
</net>
<net name="N$46" class="0">
<segment>
<pinref part="R18" gate="A" pin="1"/>
<wire x1="231.14" y1="86.36" x2="226.06" y2="86.36" width="0.1524" layer="91"/>
<pinref part="LED1" gate="A" pin="CATHODE"/>
<wire x1="231.14" y1="86.36" x2="231.14" y2="88.9" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<pinref part="J2" gate="A" pin="1"/>
<wire x1="309.88" y1="170.18" x2="312.42" y2="170.18" width="0.1524" layer="91"/>
<pinref part="U1" gate="A" pin="XTAL1_CLKIN"/>
<wire x1="261.62" y1="198.12" x2="261.62" y2="195.58" width="0.1524" layer="91"/>
<wire x1="261.62" y1="195.58" x2="309.88" y2="195.58" width="0.1524" layer="91"/>
<wire x1="309.88" y1="195.58" x2="309.88" y2="170.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$36" class="0">
<segment>
<pinref part="U1" gate="A" pin="SCLK"/>
<wire x1="261.62" y1="167.64" x2="289.56" y2="167.64" width="0.1524" layer="91"/>
<wire x1="289.56" y1="167.64" x2="289.56" y2="162.56" width="0.1524" layer="91"/>
<pinref part="J2" gate="A" pin="4"/>
<wire x1="289.56" y1="162.56" x2="312.42" y2="162.56" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$38" class="0">
<segment>
<pinref part="U1" gate="A" pin="DRDY_N"/>
<wire x1="261.62" y1="162.56" x2="287.02" y2="162.56" width="0.1524" layer="91"/>
<wire x1="287.02" y1="162.56" x2="287.02" y2="160.02" width="0.1524" layer="91"/>
<pinref part="J2" gate="A" pin="5"/>
<wire x1="287.02" y1="160.02" x2="312.42" y2="160.02" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>
