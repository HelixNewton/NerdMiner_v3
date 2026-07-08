#pragma once
#include <pgmspace.h>

const char DASHBOARD_HTML[] PROGMEM = R"rawdash(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>NerdMiner</title>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/@tabler/icons-webfont@latest/tabler-icons.min.css">
<style>
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
:root{
  --bg:#0d0f14;--sf:#111318;--bd:#1e2230;
  --mt:#475569;--tx:#e2e8f0;
  --gold:#f4d03f;--grn:#22c55e;--red:#ef4444;
  --blu:#60a5fa;--tel:#2dd4bf;--pur:#a78bfa;
  --sw:200px;
}
html,body{height:100%;overflow:hidden}
body{background:var(--bg);color:var(--tx);font-family:system-ui,-apple-system,sans-serif;font-size:13px;display:flex}
.sb{width:var(--sw);min-width:var(--sw);height:100vh;background:var(--sf);border-right:.5px solid var(--bd);display:flex;flex-direction:column;flex-shrink:0}
.sb-logo{padding:18px 16px 16px;border-bottom:.5px solid var(--bd);display:flex;align-items:center;gap:9px;font-size:15px;font-weight:600}
.sb-logo i{color:var(--gold);font-size:20px}
.sb-nav{flex:1;padding:6px 0;overflow-y:auto}
.nav-grp{padding:14px 16px 3px;font-size:10px;letter-spacing:.7px;text-transform:uppercase;color:var(--mt)}
.nav-item{display:flex;align-items:center;gap:10px;padding:7px 16px;font-size:13px;color:var(--mt);cursor:pointer;border-left:2px solid transparent;transition:color .15s,background .15s}
.nav-item:hover{color:var(--tx);background:rgba(255,255,255,.03)}
.nav-item.active{color:var(--tx);border-left-color:var(--blu);background:rgba(96,165,250,.06)}
.nav-item i{font-size:15px;flex-shrink:0}
.sb-foot{padding:14px 12px;border-top:.5px solid var(--bd)}
.hp{display:flex;align-items:center;gap:6px;background:rgba(34,197,94,.1);border:.5px solid var(--grn);border-radius:6px;padding:6px 10px;font-size:11px;color:var(--grn)}
.hp-dot{width:6px;height:6px;border-radius:50%;background:var(--grn);flex-shrink:0}
.hp-sub{font-size:10px;color:var(--mt);margin-top:5px;text-align:center}
.main{flex:1;display:flex;flex-direction:column;height:100vh;overflow:hidden;min-width:0}
.tb{height:56px;min-height:56px;background:var(--sf);border-bottom:.5px solid var(--bd);display:flex;align-items:center;padding:0 20px;gap:10px;flex-shrink:0;z-index:10}
.tb-t{font-size:15px;font-weight:600}
.tb-s{font-size:11px;color:var(--mt);margin-left:3px}
.tb-sp{flex:1}
.badge{display:inline-flex;align-items:center;gap:5px;padding:3px 9px;border-radius:6px;font-size:11px;font-weight:500}
.b-ok{background:rgba(34,197,94,.12);border:.5px solid var(--grn);color:var(--grn)}
.b-err{background:rgba(239,68,68,.12);border:.5px solid var(--red);color:var(--red)}
.b-ver{background:rgba(96,165,250,.1);border:.5px solid rgba(96,165,250,.3);color:var(--blu)}
.bdot{width:6px;height:6px;border-radius:50%;background:currentColor;flex-shrink:0}
.pool-sel{background:var(--bg);border:.5px solid var(--bd);color:var(--tx);font-size:12px;padding:4px 8px;border-radius:6px;cursor:pointer;font-family:inherit;max-width:160px}
.pool-sel:focus{outline:none;border-color:var(--blu)}
.ct{flex:1;overflow-y:auto;padding:16px 20px;display:flex;flex-direction:column;gap:14px;min-height:0}
.sr{display:grid;grid-template-columns:repeat(6,1fr);gap:10px}
.sc{background:var(--sf);border:.5px solid var(--bd);border-radius:10px;padding:14px 14px 12px;border-top-width:2px;border-top-style:solid}
.sc-lbl{display:flex;align-items:center;gap:5px;font-size:10px;text-transform:uppercase;letter-spacing:.7px;color:var(--mt);margin-bottom:8px}
.sc-lbl i{font-size:13px}
.sc-v{font-size:20px;font-weight:500;line-height:1;margin-bottom:4px}
.sc-s{font-size:11px;color:var(--mt)}
.mr{display:grid;grid-template-columns:1fr 320px;gap:10px}
.panel{background:var(--sf);border:.5px solid var(--bd);border-radius:10px;padding:16px}
.ph{display:flex;align-items:center;justify-content:space-between;margin-bottom:14px}
.ph-t{display:flex;align-items:center;gap:8px;font-size:13px;font-weight:600}
.ph-t i{color:var(--mt);font-size:16px}
.ptabs{display:flex;gap:2px}
.ptab{padding:3px 9px;border-radius:6px;font-size:11px;cursor:pointer;color:var(--mt);border:.5px solid transparent;transition:all .15s}
.ptab:hover{color:var(--tx)}
.ptab.active{color:var(--gold);border-color:rgba(244,208,63,.3);background:rgba(244,208,63,.07)}
.csv-wrap{position:relative;height:140px}
svg.csv{width:100%;height:100%;display:block;overflow:visible}
.cstrip{display:grid;grid-template-columns:repeat(5,1fr);border-top:.5px solid var(--bd);margin-top:12px;padding-top:10px}
.ci{text-align:center}
.ci-l{font-size:10px;text-transform:uppercase;letter-spacing:.7px;color:var(--mt);margin-bottom:3px}
.ci-v{font-size:12px;font-weight:500}
.pp-hdr{display:flex;align-items:flex-start;gap:10px;margin-bottom:12px;padding-bottom:12px;border-bottom:.5px solid var(--bd)}
.pp-icon{width:34px;height:34px;background:rgba(96,165,250,.1);border:.5px solid rgba(96,165,250,.3);border-radius:8px;display:flex;align-items:center;justify-content:center;flex-shrink:0}
.pp-icon i{color:var(--blu);font-size:17px}
.pp-name{font-size:13px;font-weight:600;line-height:1.3}
.pp-url{font-size:11px;color:var(--mt);margin-top:2px;word-break:break-all}
.pr{display:flex;justify-content:space-between;align-items:center;padding:5px 0;border-bottom:.5px solid var(--bd)}
.pr:last-of-type{border-bottom:none}
.pr-k{font-size:12px;color:var(--mt)}
.pr-v{font-size:12px;font-weight:500;text-align:right;max-width:170px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.vg{color:var(--grn)}.vr{color:var(--red)}.vb{color:var(--gold)}
.dbars{display:flex;align-items:flex-end;gap:3px;height:30px;margin-top:12px;padding-top:12px;border-top:.5px solid var(--bd)}
.dbar{flex:1;background:rgba(96,165,250,.2);border-radius:2px}
.dbar.lat{background:var(--blu)}
.dbars-lbl{font-size:10px;color:var(--mt);margin-top:5px}
.br{display:grid;grid-template-columns:repeat(3,1fr);gap:10px}
.bp{background:var(--sf);border:.5px solid var(--bd);border-radius:10px;padding:16px}
.st{width:100%;border-collapse:collapse;margin-top:8px}
.st th{font-size:10px;text-transform:uppercase;letter-spacing:.7px;color:var(--mt);padding:0 0 8px;font-weight:500;text-align:left;border-bottom:.5px solid var(--bd)}
.st th:last-child,.st td:last-child{text-align:right}
.st td{padding:6px 0;font-size:12px;border-bottom:.5px solid rgba(30,34,48,.5)}
.st tr:last-child td{border-bottom:none}
.pa{display:inline-flex;align-items:center;gap:3px;background:rgba(34,197,94,.1);border:.5px solid var(--grn);color:var(--grn);border-radius:6px;padding:2px 6px;font-size:10px;font-weight:500}
.pj{display:inline-flex;align-items:center;gap:3px;background:rgba(239,68,68,.1);border:.5px solid var(--red);color:var(--red);border-radius:6px;padding:2px 6px;font-size:10px;font-weight:500}
.sr2{display:flex;justify-content:space-between;align-items:center;padding:6px 0;border-bottom:.5px solid rgba(30,34,48,.5)}
.sr2:last-child{border-bottom:none}
.sr2-k{font-size:12px;color:var(--mt)}
.sr2-v{font-size:12px;font-weight:500;text-align:right}
.mb-t{width:80px;height:5px;background:var(--bd);border-radius:3px;overflow:hidden;margin-top:3px;margin-left:auto}
.mb-f{height:100%;background:var(--grn);border-radius:3px;transition:width .5s}
.ab{width:100%;display:flex;align-items:center;gap:10px;padding:9px 12px;border-radius:6px;border:.5px solid var(--bd);background:transparent;color:var(--tx);font-size:13px;font-family:inherit;cursor:pointer;transition:border-color .15s,color .15s,background .15s;margin-bottom:6px;text-align:left}
.ab:last-of-type{margin-bottom:0}
.ab i{font-size:15px;flex-shrink:0;color:var(--mt)}
.ab:hover{border-color:var(--blu);background:rgba(96,165,250,.05);color:var(--blu)}
.ab:hover i{color:var(--blu)}
.ab.danger:hover{border-color:var(--red);background:rgba(239,68,68,.05);color:var(--red)}
.ab.danger:hover i{color:var(--red)}
.coffee{display:flex;align-items:center;gap:8px;margin-top:8px;padding:8px 12px;border-radius:6px;border:.5px solid rgba(244,208,63,.3);background:rgba(244,208,63,.05);color:var(--gold);font-size:13px;text-decoration:none;transition:background .15s;font-family:inherit}
.coffee:hover{background:rgba(244,208,63,.1)}
.coffee i{font-size:15px;flex-shrink:0}
.ft{height:36px;min-height:36px;background:var(--sf);border-top:.5px solid var(--bd);display:flex;align-items:center;justify-content:space-between;padding:0 20px;font-size:11px;color:var(--mt);flex-shrink:0}
.toast{position:fixed;top:14px;right:14px;z-index:999;padding:9px 14px;border-radius:6px;font-size:12px;opacity:0;pointer-events:none;transition:opacity .25s;max-width:280px}
.toast.show{opacity:1}
.t-ok{background:#0c1f14;border:.5px solid var(--grn);color:var(--grn)}
.t-err{background:#1c0e0e;border:.5px solid var(--red);color:var(--red)}
.t-warn{background:#1c1708;border:.5px solid var(--gold);color:var(--gold)}
.mo{position:fixed;inset:0;background:rgba(0,0,0,.75);z-index:100;display:none;align-items:center;justify-content:center}
.mo.show{display:flex}
.md{background:var(--sf);border:.5px solid var(--bd);border-radius:10px;width:440px;max-width:95vw;max-height:90vh;overflow-y:auto}
.md-h{display:flex;align-items:center;justify-content:space-between;padding:14px 16px;border-bottom:.5px solid var(--bd)}
.md-t{font-size:14px;font-weight:600;display:flex;align-items:center;gap:8px}
.md-t i{color:var(--mt)}
.md-x{background:none;border:none;color:var(--mt);cursor:pointer;font-size:20px;padding:0 3px;line-height:1}
.md-x:hover{color:var(--tx)}
.md-b{padding:16px}
.md-f{display:flex;gap:8px;justify-content:flex-end;padding:12px 16px;border-top:.5px solid var(--bd)}
.fg{margin-bottom:12px}
.fl{display:block;font-size:11px;color:var(--mt);margin-bottom:5px;text-transform:uppercase;letter-spacing:.5px}
.fi{width:100%;background:var(--bg);border:.5px solid var(--bd);color:var(--tx);font-family:inherit;font-size:13px;padding:7px 10px;border-radius:6px;outline:none;transition:border-color .15s}
.fi:focus{border-color:var(--blu)}
.fr2{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.fc{display:flex;align-items:center;gap:8px;margin-top:4px}
.fc input{accent-color:var(--grn);width:14px;height:14px;cursor:pointer}
.fc label{font-size:13px;cursor:pointer}
.btn{padding:7px 14px;border-radius:6px;font-size:13px;font-family:inherit;cursor:pointer;border:.5px solid transparent;transition:all .15s;font-weight:500}
.btn-p{background:var(--blu);color:#0d0f14;border-color:var(--blu)}
.btn-p:hover{background:#93c5fd;border-color:#93c5fd}
.btn-g{background:transparent;color:var(--tx);border-color:var(--bd)}
.btn-g:hover{border-color:var(--tx)}
.op-w{margin-top:10px;display:none}
.pt{height:6px;background:var(--bd);border-radius:3px;overflow:hidden}
.pf{height:100%;width:0%;background:var(--blu);border-radius:3px;transition:width .3s}
.pm{font-size:11px;color:var(--mt);margin-top:6px}
/* event log rows */
.log-row{display:flex;gap:10px;padding:7px 0;border-bottom:.5px solid rgba(30,34,48,.5);align-items:flex-start}
.log-row:last-child{border-bottom:none}
.log-ts{font-size:11px;color:var(--mt);white-space:nowrap;min-width:76px;padding-top:1px}
.log-msg{font-size:12px}
.ev-ok{color:var(--grn)}.ev-warn{color:var(--gold)}.ev-err{color:var(--red)}.ev-inf{color:var(--blu)}
.empty-state{color:var(--mt);padding:28px 0;text-align:center;font-size:12px}
/* view switching — set class on #ct */
.cv-workers .mr{display:none}
.cv-workers .br{grid-template-columns:1fr}
.cv-workers .bp-shares,.cv-workers .bp-actions{display:none}
.cv-pools .sr{display:none}
.cv-pools .mr{grid-template-columns:1fr}
.cv-pools .chart-panel{display:none}
.cv-pools .br{display:none}
.cv-stats .mr{grid-template-columns:1fr}
.cv-stats .pool-panel{display:none}
.cv-stats .br{display:none}
.cv-shares .sr{display:none}
.cv-shares .mr{display:none}
.cv-shares .br{grid-template-columns:1fr}
.cv-shares .bp-system,.cv-shares .bp-actions{display:none}
.cv-alerts .sr,.cv-alerts .mr,.cv-alerts .br{display:none}
.cv-logs .sr,.cv-logs .mr,.cv-logs .br{display:none}
.cv-fleet .sr,.cv-fleet .mr,.cv-fleet .br{display:none}
.fl-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:10px;margin-bottom:14px}
.fl-link{color:var(--blu);text-decoration:none}
.fl-link:hover{text-decoration:underline}
.lnk-x{background:none;border:none;color:var(--mt);cursor:pointer;font-size:14px;padding:0 3px;line-height:1}
.lnk-x:hover{color:var(--red)}
@media(max-width:680px){.fl-grid{grid-template-columns:1fr}}
@media(max-width:1200px){.sr{grid-template-columns:repeat(3,1fr)}.mr{grid-template-columns:1fr}}
@media(max-width:900px){.br{grid-template-columns:1fr 1fr}}
@media(max-width:680px){.sb{display:none}.sr{grid-template-columns:repeat(2,1fr)}.br{grid-template-columns:1fr}}
</style>
</head>
<body>

<aside class="sb">
  <div class="sb-logo"><i class="ti ti-bolt"></i>NerdMiner</div>
  <nav class="sb-nav">
    <div class="nav-grp">Mining</div>
    <div class="nav-item active" data-v="overview" onclick="nav(this)"><i class="ti ti-layout-dashboard"></i>Overview</div>
    <div class="nav-item" data-v="fleet" onclick="nav(this)"><i class="ti ti-network"></i>Fleet</div>
    <div class="nav-item" data-v="workers" onclick="nav(this)"><i class="ti ti-cpu"></i>Workers</div>
    <div class="nav-item" data-v="pools" onclick="nav(this)"><i class="ti ti-server"></i>Pools</div>
    <div class="nav-grp">Analytics</div>
    <div class="nav-item" data-v="stats" onclick="nav(this)"><i class="ti ti-chart-bar"></i>Statistics</div>
    <div class="nav-item" data-v="shares" onclick="nav(this)"><i class="ti ti-coins"></i>Shares</div>
    <div class="nav-grp">System</div>
    <div class="nav-item" data-v="alerts" onclick="nav(this)"><i class="ti ti-bell"></i>Alerts</div>
    <div class="nav-item" data-v="logs" onclick="nav(this)"><i class="ti ti-terminal"></i>Logs</div>
    <div class="nav-item" onclick="openCfg()"><i class="ti ti-settings"></i>Settings</div>
  </nav>
  <div class="sb-foot">
    <div class="hp" id="healthPill"><span class="hp-dot" id="healthDot"></span><span id="healthTxt">Healthy</span></div>
    <div class="hp-sub">All systems operational</div>
  </div>
</aside>

<div class="main">
  <header class="tb">
    <span class="tb-t" id="tbT">Overview</span>
    <span class="tb-s" id="tbSub">Live mining monitor</span>
    <div class="tb-sp"></div>
    <span class="badge b-ok" id="connBadge"><span class="bdot"></span><span id="connTxt">Connecting</span></span>
    <span class="badge b-ver" id="vpnBadge" title="WireGuard VPN tunnel" style="display:none"><i class="ti ti-shield-lock"></i> VPN</span>
    <span class="badge b-ver" id="verBadge">v&#x2014;</span>
    <select class="pool-sel" id="poolSel" title="Switch this miner to another pool" onchange="onPoolSel(this.value)">
      <option value="">&#x2014; Switch pool &#x2014;</option>
    </select>
  </header>

  <div class="ct" id="ct">

    <div class="sr">
      <div class="sc" style="border-top-color:var(--gold)">
        <div class="sc-lbl" style="color:var(--gold)"><i class="ti ti-activity"></i>Hashrate</div>
        <div class="sc-v" style="color:var(--gold)" id="scH">&#x2014;</div>
        <div class="sc-s" id="scHs">&#x2014;</div>
      </div>
      <div class="sc" style="border-top-color:var(--grn)">
        <div class="sc-lbl" style="color:var(--grn)"><i class="ti ti-circle-check"></i>Accepted</div>
        <div class="sc-v" style="color:var(--grn)" id="scA">&#x2014;</div>
        <div class="sc-s" id="scAs">shares</div>
      </div>
      <div class="sc" style="border-top-color:var(--red)">
        <div class="sc-lbl" style="color:var(--red)"><i class="ti ti-circle-x"></i>Rejected</div>
        <div class="sc-v" style="color:var(--red)" id="scR">0</div>
        <div class="sc-s">this session</div>
      </div>
      <div class="sc" style="border-top-color:var(--blu)">
        <div class="sc-lbl" style="color:var(--blu)"><i class="ti ti-clock"></i>Uptime</div>
        <div class="sc-v" style="color:var(--blu);font-size:16px" id="scU">&#x2014;</div>
        <div class="sc-s" id="scUs">&#x2014;</div>
      </div>
      <div class="sc" style="border-top-color:var(--tel)">
        <div class="sc-lbl" style="color:var(--tel)"><i class="ti ti-database"></i>Free Heap</div>
        <div class="sc-v" style="color:var(--tel);font-size:16px" id="scF">&#x2014;</div>
        <div class="sc-s" id="scFs">of total</div>
      </div>
      <div class="sc" style="border-top-color:var(--pur)">
        <div class="sc-lbl" style="color:var(--pur)"><i class="ti ti-cpu"></i>Templates</div>
        <div class="sc-v" style="color:var(--pur)" id="scT">&#x2014;</div>
        <div class="sc-s">jobs received</div>
      </div>
    </div>

    <div class="mr">
      <div class="panel chart-panel">
        <div class="ph">
          <div class="ph-t"><i class="ti ti-chart-line"></i>Hashrate history</div>
          <div class="ptabs">
            <span class="ptab" onclick="setTab(this,'1h')">1H</span>
            <span class="ptab" onclick="setTab(this,'6h')">6H</span>
            <span class="ptab active" onclick="setTab(this,'24h')">24H</span>
            <span class="ptab" onclick="setTab(this,'7d')">7D</span>
            <span class="ptab" onclick="setTab(this,'30d')">30D</span>
          </div>
        </div>
        <div class="csv-wrap">
          <svg class="csv" id="hrSvg" preserveAspectRatio="none">
            <defs>
              <linearGradient id="cg" x1="0" y1="0" x2="0" y2="1">
                <stop offset="0%" stop-color="#f4d03f" stop-opacity="0.2"/>
                <stop offset="100%" stop-color="#f4d03f" stop-opacity="0"/>
              </linearGradient>
            </defs>
            <path id="hrFill" fill="url(#cg)" stroke="none"/>
            <path id="hrLine" fill="none" stroke="#f4d03f" stroke-width="1.5" stroke-linejoin="round" stroke-linecap="round"/>
          </svg>
        </div>
        <div class="cstrip">
          <div class="ci"><div class="ci-l">Current</div><div class="ci-v" id="csN">&#x2014;</div></div>
          <div class="ci"><div class="ci-l">Avg</div><div class="ci-v" id="csA">&#x2014;</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--grn)">High</div><div class="ci-v" style="color:var(--grn)" id="csHi">&#x2014;</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--red)">Low</div><div class="ci-v" style="color:var(--red)" id="csLo">&#x2014;</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--gold)">Variance</div><div class="ci-v" style="color:var(--gold)" id="csV">&#x2014;</div></div>
        </div>
      </div>

      <div class="panel pool-panel">
        <div class="ph"><div class="ph-t"><i class="ti ti-server"></i>Pool info</div></div>
        <div class="pp-hdr">
          <div class="pp-icon"><i class="ti ti-cloud-computing"></i></div>
          <div>
            <div class="pp-name" id="ppName">&#x2014;</div>
            <div class="pp-url" id="ppUrl">&#x2014;</div>
          </div>
        </div>
        <div class="pr"><span class="pr-k">Status</span><span class="pr-v vg" id="ppSt">&#x2014;</span></div>
        <div class="pr"><span class="pr-k">Last share</span><span class="pr-v" id="ppLs">&#x2014;</span></div>
        <div class="pr"><span class="pr-k">Best difficulty</span><span class="pr-v vb" id="ppDf">&#x2014;</span></div>
        <div class="pr"><span class="pr-k">Worker</span><span class="pr-v" id="ppWk">&#x2014;</span></div>
        <div class="pr"><span class="pr-k">Wallet</span><span class="pr-v" id="ppWa">&#x2014;</span></div>
        <div class="dbars" id="dbars"></div>
        <div class="dbars-lbl">Difficulty trend (last 7 samples)</div>
      </div>
    </div>

    <div class="br">
      <div class="bp bp-shares">
        <div class="ph"><div class="ph-t"><i class="ti ti-list-check"></i>Recent shares</div></div>
        <table class="st">
          <thead><tr><th>Time</th><th>Result</th><th>Difficulty</th></tr></thead>
          <tbody id="sTbody"><tr><td colspan="3" style="color:var(--mt);text-align:center;padding:14px 0">Waiting for shares&#x2026;</td></tr></tbody>
        </table>
      </div>

      <div class="bp bp-system">
        <div class="ph"><div class="ph-t"><i class="ti ti-device-desktop"></i>System</div></div>
        <div class="sr2"><span class="sr2-k">Chip</span><span class="sr2-v" id="sChip">&#x2014;</span></div>
        <div class="sr2"><span class="sr2-k">CPU</span><span class="sr2-v" id="sCpu">&#x2014;</span></div>
        <div class="sr2">
          <span class="sr2-k">Memory</span>
          <div><div class="sr2-v" id="sMem">&#x2014;</div><div class="mb-t"><div class="mb-f" id="sMemB" style="width:0%"></div></div></div>
        </div>
        <div class="sr2"><span class="sr2-k">Flash</span><span class="sr2-v" id="sFlash">&#x2014;</span></div>
        <div class="sr2"><span class="sr2-k">MAC</span><span class="sr2-v" id="sMac">&#x2014;</span></div>
        <div class="sr2"><span class="sr2-k">SDK</span><span class="sr2-v" id="sSdk">&#x2014;</span></div>
        <div class="sr2"><span class="sr2-k">IP address</span><span class="sr2-v vg" id="sIp">&#x2014;</span></div>
      </div>

      <div class="bp bp-actions">
        <div class="ph"><div class="ph-t"><i class="ti ti-settings-2"></i>Quick actions</div></div>
        <button class="ab" onclick="openCfg()"><i class="ti ti-adjustments"></i>Configure miner</button>
        <button class="ab" onclick="openOta()"><i class="ti ti-upload"></i>Firmware update (OTA)</button>
        <button class="ab" onclick="testPool()"><i class="ti ti-plug-connected"></i>Test pool connection</button>
        <button class="ab" onclick="restartDevice()"><i class="ti ti-refresh"></i>Restart miner</button>
        <button class="ab danger" onclick="factoryReset()"><i class="ti ti-trash"></i>Factory reset</button>
        <a class="coffee" href="https://buymeacoffee.com/sx8yfh9zrbs" target="_blank" rel="noopener"><i class="ti ti-coffee"></i>Buy me a coffee</a>
      </div>
    </div>

    <div id="vAlerts" class="panel" style="display:none">
      <div class="ph">
        <div class="ph-t"><i class="ti ti-bell"></i>Alerts &amp; events</div>
        <span id="alertCount" class="badge b-ver" style="font-size:10px">0 events</span>
      </div>
      <div id="alertList"><div class="empty-state">No alerts &#x2014; all systems operational</div></div>
    </div>

    <div id="vLogs" class="panel" style="display:none">
      <div class="ph">
        <div class="ph-t"><i class="ti ti-terminal"></i>System log</div>
        <span id="logCount" class="badge b-ver" style="font-size:10px">0 entries</span>
      </div>
      <div id="logList"><div class="empty-state">No log entries yet</div></div>
    </div>

    <div id="vFleet" class="panel" style="display:none">
      <div class="ph">
        <div class="ph-t"><i class="ti ti-network"></i>Fleet &#x2014; all miners</div>
        <span id="fleetCount" class="badge b-ver" style="font-size:10px">0 miners</span>
      </div>
      <div style="display:flex;gap:8px;margin-bottom:14px">
        <input class="fi" id="fleetInput" type="text" placeholder="Add miner by IP or hostname (e.g. 192.168.1.50)" style="flex:1" onkeydown="if(event.key==='Enter')fleetAddInput()">
        <button class="btn btn-p" onclick="fleetAddInput()">Add</button>
        <button class="btn btn-g" id="fleetScanBtn" onclick="fleetScan()">Scan LAN</button>
        <button class="btn btn-g" onclick="fleetRefresh()">Refresh</button>
        <button class="btn btn-g" id="fleetRestartBtn" onclick="fleetRestartAll()">Restart all</button>
        <button class="btn btn-g" id="fleetOtaBtn" onclick="fleetUpdateAll()">Update all</button>
        <input type="file" id="fleetOtaFile" accept=".bin" style="display:none" onchange="fleetOtaPicked()">
      </div>
      <div class="fl-grid">
        <div class="sc" style="border-top-color:var(--grn)"><div class="sc-lbl" style="color:var(--grn)"><i class="ti ti-cpu"></i>Online</div><div class="sc-v" style="color:var(--grn)" id="flOnline">&#x2014;</div><div class="sc-s">miners reachable</div></div>
        <div class="sc" style="border-top-color:var(--gold)"><div class="sc-lbl" style="color:var(--gold)"><i class="ti ti-activity"></i>Total hashrate</div><div class="sc-v" style="color:var(--gold)" id="flHash">&#x2014;</div><div class="sc-s">combined</div></div>
        <div class="sc" style="border-top-color:var(--blu)"><div class="sc-lbl" style="color:var(--blu)"><i class="ti ti-circle-check"></i>Total accepted</div><div class="sc-v" style="color:var(--blu)" id="flShares">&#x2014;</div><div class="sc-s">shares</div></div>
      </div>
      <table class="st">
        <thead><tr><th>Miner</th><th>IP</th><th>Status</th><th>Hashrate</th><th>Accepted</th><th>Best diff</th><th>Uptime</th><th>Version</th><th></th></tr></thead>
        <tbody id="fleetBody"><tr><td colspan="9" class="empty-state">No miners yet &#x2014; add one above. This device is added automatically.</td></tr></tbody>
      </table>
      <div id="fleetOtaPanel" style="display:none;margin-top:10px"></div>
      <div class="dbars-lbl" style="margin-top:10px"><strong>Scan LAN</strong> auto-discovers miners on this device&#x2019;s subnet. Stats are polled directly from each miner&#x2019;s <code>/api/status</code> &#x2014; all miners must be on this same network. <strong>Update all</strong> pushes one firmware <code>.bin</code> to every miner over WiFi (this device flashes last; requires an OTA-capable partition table).</div>
    </div>

  </div>

  <footer class="ft">
    <span id="ftL">Last updated &#x2014;</span>
    <span id="ftC">Timezone UTC+0 &#xB7; Auto-refresh 3s</span>
    <span id="ftR">Save stats to flash (NVS): &#x2014;</span>
  </footer>
</div>

<div class="toast" id="toast"></div>

<div class="mo" id="moCfg">
  <div class="md">
    <div class="md-h">
      <div class="md-t"><i class="ti ti-adjustments"></i>Configure miner</div>
      <button class="md-x" onclick="closeM('moCfg')">&#x2715;</button>
    </div>
    <div class="md-b">
      <div class="fg"><label class="fl">Wallet address</label><input class="fi" id="cWallet" type="text" placeholder="bc1q&#x2026; or 1&#x2026; or 3&#x2026;"></div>
      <div class="fr2">
        <div class="fg"><label class="fl">Pool URL</label><input class="fi" id="cUrl" type="text" placeholder="public-pool.io"></div>
        <div class="fg"><label class="fl">Pool port</label><input class="fi" id="cPort" type="number" min="1" max="65535" placeholder="21496"></div>
      </div>
      <div class="fg"><label class="fl">Pool password</label><input class="fi" id="cPass" type="text" placeholder="x"></div>
      <div class="fg" style="max-width:160px"><label class="fl">Timezone (UTC offset)</label><input class="fi" id="cTz" type="number" min="-12" max="14" placeholder="0"></div>
      <div class="fc"><input type="checkbox" id="cSave"><label for="cSave">Save mining stats to flash (NVS)</label></div>
      <div class="fg"><label class="fl">API token &#x2014; only for firmware built with <code>WEBUI_AUTH_TOKEN</code>; saved in this browser, never in the miner&#x2019;s config</label><input class="fi" id="cToken" type="password" placeholder="leave empty if unused" onchange="setToken(this.value)"></div>
      <div id="wgSection" style="display:none;border-top:1px solid var(--bd);margin-top:14px;padding-top:12px">
        <div class="fc"><input type="checkbox" id="cWgEn"><label for="cWgEn"><strong>WireGuard VPN</strong> &#x2014; full-tunnel (pool traffic + remote dashboard access)</label></div>
        <div class="fr2">
          <div class="fg"><label class="fl">Tunnel IP (this device)</label><input class="fi" id="cWgIp" type="text" placeholder="10.6.0.2"></div>
          <div class="fg"><label class="fl">Endpoint port</label><input class="fi" id="cWgPort" type="number" min="1" max="65535" placeholder="51820"></div>
        </div>
        <div class="fg"><label class="fl">Server endpoint (host or IP)</label><input class="fi" id="cWgEp" type="text" placeholder="vpn.example.com"></div>
        <div class="fg"><label class="fl">Server public key</label><input class="fi" id="cWgPub" type="text" placeholder="base64 peer public key"></div>
        <div class="fg"><label class="fl">This device&#x2019;s private key <span id="cWgPkState" style="color:var(--mt)"></span></label><input class="fi" id="cWgPriv" type="password" placeholder="base64 private key"></div>
        <div style="font-size:11px;color:var(--mt)">Keys are stored on the miner and used only for the tunnel. The private key is never sent back to the browser; leave it blank to keep the current one. Saving restarts the miner.</div>
      </div>
      <div id="cMsg" style="font-size:12px;color:var(--mt);margin-top:10px"></div>
    </div>
    <div class="md-f">
      <button class="btn btn-g" onclick="closeM('moCfg')">Cancel</button>
      <button class="btn btn-p" onclick="saveCfg()">Save &amp; restart</button>
    </div>
  </div>
</div>

<div class="mo" id="moOta">
  <div class="md">
    <div class="md-h">
      <div class="md-t"><i class="ti ti-upload"></i>Firmware update (OTA)</div>
      <button class="md-x" onclick="closeM('moOta')">&#x2715;</button>
    </div>
    <div class="md-b">
      <p style="font-size:12px;color:var(--mt);margin-bottom:12px">Upload a compiled <strong style="color:var(--tx)">.bin</strong> file. Device restarts automatically. Current firmware: <span id="otaV" style="color:var(--blu)">&#x2014;</span></p>
      <div style="display:flex;gap:8px;align-items:center">
        <input class="fi" type="file" id="otaFile" accept=".bin" style="flex:1">
        <button class="btn btn-p" onclick="startOta()">Flash</button>
      </div>
      <div class="op-w" id="otaW">
        <div class="pt"><div class="pf" id="otaFill"></div></div>
        <div class="pm" id="otaMsg">Uploading&#x2026;</div>
      </div>
    </div>
    <div class="md-f"><button class="btn btn-g" onclick="closeM('moOta')">Close</button></div>
  </div>
</div>

<script>
(function(){
'use strict';
var POLL=3000,MAX_HR=120;
var hrBuf=[],rejected=0,shareLog=[],lastShare=null,tz=0,saveStats=false;
var diffHist=[0,0,0,0,0,0,0],lastTs=null;
var alertLog=[],sysLog=[],prevConn=null;
var currentView='overview',activePool='',lastWallet='';
// Fleet (multi-miner) state — host list persisted on-device (NVS) so every
// browser that opens this miner sees the same fleet; localStorage is only a
// same-tab instant-paint cache.
var fleet=[],fleetData={},fleetBusy=false,selfIp=null,lastStatus=null,selfAdded=false,scanning=false;
var fleetLoaded=false,fleetPendingSelf=null,FLEET_MAX=32;
// Pool list used to populate the switcher; replaced by GET /api/pools when available
var POOLS_FALLBACK=[
  {name:'public-pool.io',host:'public-pool.io',port:21496},
  {name:'nerdminers.org',host:'pool.nerdminers.org',port:3333},
  {name:'sethforprivacy.com',host:'pool.sethforprivacy.com',port:3333},
  {name:'solomining.de',host:'pool.solomining.de',port:3333},
  {name:'pyblock.xyz',host:'pool.pyblock.xyz',port:3333}
];

var VIEWS={
  overview:{t:'Overview',s:'Live mining monitor'},
  fleet:{t:'Fleet',s:'All miners on your network'},
  workers:{t:'Workers',s:'Single-device miner stats'},
  pools:{t:'Pools',s:'Pool connection & status'},
  stats:{t:'Statistics',s:'Hashrate analytics'},
  shares:{t:'Shares',s:'Submission log'},
  alerts:{t:'Alerts',s:'Events & notifications'},
  logs:{t:'Logs',s:'System activity log'}
};

function el(id){return document.getElementById(id);}
function set(id,v){var e=el(id);if(e)e.textContent=v;}

function fmtH(k){
  if(k>=1000)return (k/1000).toFixed(2)+' MH/s';
  if(k>=1)return k.toFixed(1)+' KH/s';
  return Math.round(k*1000)+' H/s';
}
function fmtB(b){
  if(b>=1048576)return (b/1048576).toFixed(1)+' MB';
  if(b>=1024)return Math.round(b/1024)+' KB';
  return b+' B';
}
function fmtN(n){return n?Number(n).toLocaleString():'0';}
function fmtUp(s){
  var d=Math.floor(s/86400),h=Math.floor((s%86400)/3600),m=Math.floor((s%3600)/60);
  if(d>0)return d+'d '+h+'h';
  if(h>0)return h+'h '+m+'m';
  return m+'m '+(s%60)+'s';
}
function fmtD(v){
  var n=parseFloat(v)||0;
  if(!n)return '0';
  if(n>=1e6)return (n/1e6).toFixed(2)+'M';
  if(n>=1e3)return (n/1e3).toFixed(2)+'K';
  if(n>=1)return n.toFixed(4);
  return n.toExponential(3);
}
function trunc(s,n){if(!s||s.length<=n)return s||'--';return s.slice(0,6)+'...'+s.slice(-6);}
function nowT(){return new Date().toLocaleTimeString();}

var tTmr;
function toast(msg,type){
  var t=el('toast');
  t.textContent=msg;
  t.className='toast t-'+(type||'ok')+' show';
  clearTimeout(tTmr);
  tTmr=setTimeout(function(){t.classList.remove('show');},3200);
}

function nav(item){
  document.querySelectorAll('.nav-item').forEach(function(n){n.classList.remove('active');});
  item.classList.add('active');
  var v=item.dataset.v||'overview';
  currentView=v;
  el('ct').className='ct cv-'+v;
  el('vAlerts').style.display=(v==='alerts')?'block':'none';
  el('vLogs').style.display=(v==='logs')?'block':'none';
  el('vFleet').style.display=(v==='fleet')?'block':'none';
  var info=VIEWS[v]||VIEWS.overview;
  set('tbT',info.t);
  set('tbSub',info.s);
  renderChart();
  if(v==='fleet')fleetRefresh();
}
window.nav=nav;

function setTab(e,t){
  document.querySelectorAll('.ptab').forEach(function(x){x.classList.remove('active');});
  e.classList.add('active');
  renderChart();
}
window.setTab=setTab;

function renderChart(){
  var wrap=document.querySelector('.csv-wrap');
  var svg=el('hrSvg');
  if(!svg||!wrap)return;
  var W=wrap.clientWidth||600,H=wrap.clientHeight||140;
  svg.setAttribute('viewBox','0 0 '+W+' '+H);
  var data=hrBuf.slice(-MAX_HR);
  if(data.length<2){el('hrFill').setAttribute('d','');el('hrLine').setAttribute('d','');return;}
  var mx=Math.max.apply(null,data)||1,pad=12,cH=H-pad;
  svg.querySelectorAll('.gl').forEach(function(g){g.remove();});
  for(var i=1;i<4;i++){
    var y=Math.round(cH*i/4)+0.5;
    var ln=document.createElementNS('http://www.w3.org/2000/svg','line');
    ln.setAttribute('x1','0');ln.setAttribute('x2',W);
    ln.setAttribute('y1',y);ln.setAttribute('y2',y);
    ln.setAttribute('stroke','#1e2230');ln.setAttribute('stroke-dasharray','4 4');ln.setAttribute('stroke-width','0.5');
    ln.classList.add('gl');
    svg.insertBefore(ln,svg.firstChild);
  }
  var pts=data.map(function(v,i){
    return [((i/(data.length-1))*W).toFixed(1),(cH-(v/mx)*(cH-pad)-pad/2).toFixed(1)];
  });
  var ld='M'+pts.map(function(p){return p[0]+','+p[1];}).join('L');
  el('hrLine').setAttribute('d',ld);
  el('hrFill').setAttribute('d',ld+'L'+pts[pts.length-1][0]+','+cH+'L0,'+cH+'Z');
  var avg=data.reduce(function(a,b){return a+b;},0)/data.length;
  var hi=Math.max.apply(null,data),lo=Math.min.apply(null,data);
  set('csN',fmtH(data[data.length-1]));
  set('csA',fmtH(avg));
  set('csHi',fmtH(hi));
  set('csLo',fmtH(lo));
  set('csV','+/-'+fmtH((hi-lo)/2));
}

function renderDbars(){
  var c=el('dbars');if(!c)return;
  var mx=Math.max.apply(null,diffHist)||1;
  c.innerHTML=diffHist.map(function(v,i){
    var h=Math.max(3,Math.round((v/mx)*28));
    return '<div class="dbar'+(i===diffHist.length-1?' lat':'')+'" style="height:'+h+'px"></div>';
  }).join('');
}

async function loadPools(){
  var pools=POOLS_FALLBACK;
  try{var r=await api('/api/pools');if(r.ok){var j=await r.json();if(Array.isArray(j)&&j.length)pools=j;}}catch(e){}
  var sel=el('poolSel');if(!sel)return;
  sel.innerHTML='<option value="">— Switch pool —</option>'+pools.map(function(p){
    return '<option value="'+p.host+':'+p.port+'">'+(p.name||p.host)+'</option>';
  }).join('');
  if(activePool)sel.value=activePool;
}

// Switch this miner to the selected pool by reusing the validated /api/config
// POST (which persists settings and restarts the device).
async function onPoolSel(v){
  var sel=el('poolSel');
  if(!v){return;}
  var i=v.lastIndexOf(':'),host=v.slice(0,i),port=parseInt(v.slice(i+1))||3333;
  if(!confirm('Switch this miner to '+host+':'+port+'?\n\nThe new pool is saved and the device restarts.')){
    sel.value=activePool||'';return;
  }
  toast('Switching to '+host+'…','warn');
  try{
    var cfgR=await api('/api/config');var cfg=cfgR.ok?await cfgR.json():{};
    var wallet=((cfg.wallet||lastWallet||'')+'').trim();
    if(!wallet){toast('No wallet set — open Settings first','err');sel.value=activePool||'';return;}
    var body={wallet:wallet,pool_url:host,pool_port:port,
      pool_pass:cfg.pool_pass||'x',
      timezone:(cfg.timezone!=null?cfg.timezone:tz),
      save_stats:!!cfg.save_stats};
    var r=await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
    var d=await r.json();
    if(d.success){toast('Pool switched — device restarting…','ok');pushLog('Pool switched to '+host+':'+port,'ok');}
    else{toast('Switch failed: '+(d.error||'unknown'),'err');sel.value=activePool||'';}
  }catch(e){toast('Switch error: '+e.message,'err');sel.value=activePool||'';}
}
window.onPoolSel=onPoolSel;

// ── Fleet (multi-miner) ─────────────────────────────────────────────────────
function fleetEsc(s){
  return String(s).replace(/[&<>"']/g,function(c){
    return {'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c];
  });
}

// host or host:port — must match the device-side isValidFleetHost()
function fleetValid(h){return typeof h==='string'&&/^[A-Za-z0-9.\-]+(:\d{1,5})?$/.test(h);}

// ── API token (WEBUI_AUTH_TOKEN builds) ───────────────────────────────────
// Stored only in this browser, never on the device. One token is assumed
// across the whole fleet — peers get it on their gated endpoints too.
var apiToken='';
try{apiToken=localStorage.getItem('nm_token')||'';}catch(e){}
var tokenPrompted=false;
function authHdrs(h){h=h||{};if(apiToken)h['X-API-Token']=apiToken;return h;}
function setToken(t){
  t=(t||'').trim();
  if(t===apiToken)return;
  apiToken=t;
  try{t?localStorage.setItem('nm_token',t):localStorage.removeItem('nm_token');}catch(e){}
  toast(t?'API token saved in this browser':'API token cleared','ok');
}
window.setToken=setToken;
function on401(){
  if(tokenPrompted)return;
  tokenPrompted=true;
  var t=window.prompt('This miner requires an API token (WEBUI_AUTH_TOKEN build).\nEnter it once — it is stored only in this browser:');
  if(t&&t.trim()){setToken(t);location.reload();}
  else toast('API token required — set it in Settings','err');
}
// fetch wrapper for same-origin API calls: token header + central 401 hook
function api(url,opts){
  opts=opts||{};opts.headers=authHdrs(opts.headers);
  return fetch(url,opts).then(function(r){if(r.status===401)on401();return r;});
}

// fetch with an abort timeout — shared by every fleet network call
function fetchT(url,ms,opts){
  var ctrl=('AbortController' in window)?new AbortController():null;
  var o=opts||{};
  o.headers=authHdrs(o.headers);
  if(ctrl)o.signal=ctrl.signal;
  var tmr=ctrl?setTimeout(function(){ctrl.abort();},ms):null;
  var done=function(){if(tmr)clearTimeout(tmr);};
  return fetch(url,o).then(function(r){done();if(r.status===401)on401();return r;},function(e){done();throw e;});
}
function fleetCache(){try{localStorage.setItem('nm_fleet',JSON.stringify(fleet));}catch(e){}}

// All persistence goes through the device as {"add":[],"remove":[]} deltas,
// serialized on a promise queue so rapid edits can't reorder. The device
// merges and returns the authoritative list, which we adopt.
var fleetQueue=Promise.resolve();
function fleetMutate(adds,removes){
  fleetQueue=fleetQueue.then(function(){
    return api('/api/fleet',{method:'POST',headers:{'Content-Type':'application/json'},
        body:JSON.stringify({add:adds||[],remove:removes||[]})})
      .then(function(r){if(!r.ok)throw new Error('HTTP '+r.status);return r.json();})
      .then(function(d){
        if(d&&Array.isArray(d.hosts)){fleet=d.hosts;fleetCache();renderFleet();}
        if(d&&d.truncated)toast('Fleet limit ('+FLEET_MAX+') reached — some miners not saved','warn');
      })
      .catch(function(e){
        toast('Fleet sync failed — change kept in this browser only','warn');
        pushLog('Fleet sync failed: '+e.message,'warn');
      });
  });
  return fleetQueue;
}

// Instant paint from the localStorage cache, then reconcile with the device
// (source of truth): an array — even an empty one — is authoritative; literal
// null means never initialized, so migrate this browser's cache up once.
function fleetLoad(){
  var local=[];
  try{local=JSON.parse(localStorage.getItem('nm_fleet')||'[]');}catch(e){local=[];}
  if(!Array.isArray(local))local=[];
  local=local.filter(fleetValid).slice(0,FLEET_MAX);
  fleet=local.slice();
  if(fleet.length)renderFleet();
  fetchT('/api/fleet',4000)
    .then(function(r){if(!r.ok)throw 0;return r.json();})
    .then(function(arr){
      if(Array.isArray(arr)){fleet=arr;fleetCache();}
      else if(arr===null&&local.length)return fleetMutate(local,[]);
    })
    .catch(function(){})
    .then(fleetSyncDone,fleetSyncDone);
}
function fleetSyncDone(){
  fleetLoaded=true;
  if(fleetPendingSelf&&fleet.indexOf(fleetPendingSelf)<0){
    fleet.push(fleetPendingSelf);fleetCache();fleetMutate([fleetPendingSelf],[]);
  }
  fleetPendingSelf=null;
  renderFleet();
  if(currentView==='fleet')fleetRefresh();
}

function renderFleet(){
  var body=el('fleetBody');if(!body)return;
  set('fleetCount',fleet.length+' miner'+(fleet.length===1?'':'s'));
  if(!fleet.length){
    body.innerHTML='<tr><td colspan="9" class="empty-state">No miners yet — add one above. This device is added automatically.</td></tr>';
    set('flOnline','0');set('flHash',fmtH(0));set('flShares','0');return;
  }
  var online=0,totHash=0,totShares=0;
  // compare exact `build` ids only — legacy peers without the field would
  // otherwise show a spurious mismatch against their `firmware` fallback
  var selfBuild=String((lastStatus&&lastStatus.build)||'');
  body.innerHTML=fleet.map(function(host){
    var res=fleetData[host];
    var rm='<td style="text-align:right"><button class="lnk-x" title="Remove" data-host="'+fleetEsc(host)+'">✕</button></td>';
    if(res&&res.ok&&res.d){
      var d=res.d;online++;var khs=d.hashrate_khs||0;totHash+=khs;totShares+=(d.shares||0);
      var name=d.hostname||host,conn=d.pool_connected&&d.pool_subscribed;
      var b=String(d.build||d.firmware||'');
      var bx=String(d.build||'');
      var ver=fleetEsc(b||'—')+(selfBuild&&bx&&bx!==selfBuild?' <span class="pj" title="differs from this device">&ne;</span>':'');
      return '<tr><td><a href="http://'+fleetEsc(host)+'/" target="_blank" rel="noopener" class="fl-link">'+fleetEsc(name)+(res.self?' <span style="color:var(--mt)">(this)</span>':'')+'</a></td>'+
        '<td style="color:var(--mt)">'+fleetEsc(d.ip||host)+'</td>'+
        '<td><span class="'+(conn?'pa':'pj')+'">'+(conn?'mining':fleetEsc(d.status||'sync'))+'</span></td>'+
        '<td>'+fmtH(khs)+'</td><td>'+fmtN(d.shares)+'</td><td class="vb">'+fmtD(d.best_diff)+'</td>'+
        '<td style="color:var(--mt)">'+fmtUp(d.uptime||0)+'</td>'+
        '<td style="color:var(--mt)">'+ver+'</td>'+rm+'</tr>';
    }
    var st=res?'<span class="pj">offline</span>':'<span style="color:var(--mt)">polling…</span>';
    return '<tr><td>'+fleetEsc(host)+'</td><td style="color:var(--mt)">—</td><td>'+st+'</td>'+
      '<td style="color:var(--mt)">—</td><td style="color:var(--mt)">—</td><td style="color:var(--mt)">—</td><td style="color:var(--mt)">—</td><td style="color:var(--mt)">—</td>'+rm+'</tr>';
  }).join('');
  set('flOnline',String(online));set('flHash',fmtH(totHash));set('flShares',fmtN(totShares));
}

function fleetRemove(host){
  delete fleetData[host];
  fleet=fleet.filter(function(h){return h!==host;});
  fleetCache();renderFleet();
  fleetMutate([],[host]);
}
window.fleetRemove=fleetRemove;

function fleetAdd(host){
  host=(host||'').trim().replace(/^https?:\/\//,'').replace(/\/.*$/,'');
  if(!host){toast('Enter an IP or hostname','err');return;}
  if(!fleetValid(host)){toast('Invalid IP or hostname','err');return;}
  if(fleet.indexOf(host)>=0){toast('Already in fleet','warn');el('fleetInput').value='';return;}
  if(fleet.length>=FLEET_MAX){toast('Fleet limit ('+FLEET_MAX+') reached','err');return;}
  fleet.push(host);fleetCache();renderFleet();fleetRefresh();
  fleetMutate([host],[]);
}
window.fleetAddInput=function(){fleetAdd(el('fleetInput').value);el('fleetInput').value='';};

function fleetPoll(host){
  if(selfIp&&host===selfIp&&lastStatus)return Promise.resolve({host:host,ok:true,d:lastStatus,self:true});
  return fetchT('http://'+host+'/api/status',2500)
    .then(function(r){if(!r.ok)throw 0;return r.json();})
    .then(function(d){return {host:host,ok:true,d:d,self:(host===selfIp)};})
    .catch(function(){return {host:host,ok:false,d:null};});
}

function fleetRefresh(){
  if(!fleet.length){renderFleet();return;}
  if(fleetBusy||otaBusy)return;
  fleetBusy=true;
  Promise.all(fleet.map(fleetPoll)).then(function(results){
    results.forEach(function(res){fleetData[res.host]=res;});
    fleetBusy=false;renderFleet();
  }).catch(function(){fleetBusy=false;});
}
window.fleetRefresh=fleetRefresh;

// Derive this device's /24 subnet from its own reported IP (e.g. "192.168.1")
function deriveSubnet(){
  var ip=(lastStatus&&lastStatus.ip)||selfIp||'';
  if(!/^\d+\.\d+\.\d+\.\d+$/.test(ip))return null;
  return ip.split('.').slice(0,3).join('.');
}
// Probe one IP for a NerdMiner. Cross-origin reads only succeed for hosts that
// send CORS '*' (i.e. our miners); everything else times out or is blocked.
function scanProbe(ip){
  return fetchT('http://'+ip+'/api/status',1200)
    .then(function(r){if(!r.ok)throw 0;return r.json();})
    .then(function(d){return (d&&typeof d.hashrate_khs!=='undefined'&&typeof d.pool_subscribed!=='undefined')?ip:null;})
    .catch(function(){return null;});
}
// Sweep the local /24 in concurrency-limited batches and add any miners found.
async function fleetScan(){
  if(scanning||otaBusy)return;
  scanning=true;
  var btn=el('fleetScanBtn');if(btn){btn.disabled=true;btn.textContent='Scanning…';}
  var found=0,newHosts=[];
  function consider(h){
    if(!fleetValid(h))return;
    found++;
    if(fleet.indexOf(h)<0&&newHosts.indexOf(h)<0&&fleet.length+newHosts.length<FLEET_MAX)newHosts.push(h);
  }
  // Fast path: the miner queries mDNS on-device (peers advertise _nerdminer._tcp),
  // so discovery takes ~3s instead of sweeping 254 addresses from the browser.
  var viaMdns=false;
  try{
    var r=await api('/api/discover');
    if(r.ok){
      var d=await r.json();
      if(Array.isArray(d)&&d.length){
        viaMdns=true;
        d.forEach(function(m){if(m&&m.ip)consider(m.ip+(m.port&&m.port!==80?':'+m.port:''));});
      }
    }
  }catch(e){}
  if(!viaMdns){
    var subnet=deriveSubnet();
    if(!subnet){
      toast('Cannot determine your network — add a miner manually first','err');
      scanning=false;if(btn){btn.disabled=false;btn.textContent='Scan LAN';}
      return;
    }
    toast('Scanning '+subnet+'.0/24 …','warn');
    var ips=[];for(var i=1;i<=254;i++)ips.push(subnet+'.'+i);
    var done=0,CONC=24;
    for(var s=0;s<ips.length;s+=CONC){
      var res=await Promise.all(ips.slice(s,s+CONC).map(scanProbe));
      res.forEach(function(ip){if(ip)consider(ip);});
      done=Math.min(ips.length,s+CONC);
      if(btn)btn.textContent='Scanning… '+done+'/254';
    }
  }
  var added=newHosts.length;
  if(added){
    fleet=fleet.concat(newHosts);
    fleetCache();
    fleetMutate(newHosts,[]);
  }
  if(btn){btn.disabled=false;btn.textContent='Scan LAN';}
  scanning=false;
  toast('Scan complete — '+found+' miner'+(found===1?'':'s')+' found'+(added?(', '+added+' new'):'')+(viaMdns?' via mDNS':''),found?'ok':'warn');
  pushLog((viaMdns?'mDNS discovery':'LAN scan')+': '+found+' miner(s) found','ok');
  renderFleet();fleetRefresh();
}
window.fleetScan=fleetScan;

// self may be stored bare, with an explicit :80, or as its mDNS name — match
// all forms, and never match on the 0.0.0.0 placeholder ip reported while
// WiFi is down. Wrongly treating self as a peer flashes/restarts this device
// mid-batch instead of last.
function isSelfHost(h){
  h=String(h||'').toLowerCase();
  if(!h)return false;
  if(selfIp&&selfIp!=='0.0.0.0'&&(h===selfIp||h===selfIp+':80'))return true;
  var hn=String((lastStatus&&lastStatus.hostname)||'').toLowerCase();
  return !!hn&&(h===hn||h===hn+'.local'||h===hn+'.local:80');
}

// Restart every miner in the fleet. Peers first (in parallel, so their
// results can be reported), then this device last — restarting self kills
// the server the dashboard is talking to.
async function fleetRestartAll(){
  if(otaBusy||scanning)return;
  if(!fleet.length){toast('No miners in fleet','warn');return;}
  if(!confirm('Restart ALL '+fleet.length+' miner'+(fleet.length===1?'':'s')+' in the fleet?'))return;
  var btn=el('fleetRestartBtn');if(btn)btn.disabled=true;
  var self=fleet.filter(isSelfHost)[0]||null;
  var others=fleet.filter(function(h){return !isSelfHost(h);});
  var ok=0,fail=0;
  await Promise.all(others.map(function(h){
    return fetchT('http://'+h+'/api/restart',4000,{method:'POST'})
      .then(function(r){if(r.ok)ok++;else fail++;})
      .catch(function(){fail++;});
  }));
  var msg='Restarted '+ok+' of '+others.length+' peer'+(others.length===1?'':'s')+(fail?(' ('+fail+' failed)'):'');
  pushLog('Fleet restart: '+msg,fail?'warn':'ok');
  if(self){
    try{await api('/api/restart',{method:'POST'});msg+=' — this device restarting…';}catch(e){}
  }
  toast(msg,fail?'warn':'ok');
  if(btn)btn.disabled=false;
}
window.fleetRestartAll=fleetRestartAll;

// ── Fleet OTA: push one firmware .bin to every miner, this device last ────
var otaBusy=false;
// Progress rows are built with textContent (never innerHTML) — hostnames and
// error strings come from remote miners and must not be parsed as HTML.
function otaRow(panel,label){
  var row=document.createElement('div');row.className='log-row';
  var name=document.createElement('span');name.className='log-ts';name.textContent=label;
  var st=document.createElement('span');st.className='log-msg';st.textContent='waiting…';
  row.appendChild(name);row.appendChild(st);panel.appendChild(row);
  return st;
}
// Shared firmware-file sanity check (fleet Update-all + settings Flash).
// Advisory only — the device rejects non-firmware payloads on its own.
function fwValid(f){
  if(!/\.bin$/i.test(f.name)){toast('Select a firmware .bin file','err');return false;}
  if(f.size<102400){toast('That file is too small to be NerdMiner firmware','err');return false;}
  return true;
}
// XHR instead of fetch: upload.onprogress is the only way to show % uploaded.
function otaUpload(url,file,onProg){
  return new Promise(function(resolve){
    var xhr=new XMLHttpRequest();
    xhr.open('POST',url);
    if(apiToken)xhr.setRequestHeader('X-API-Token',apiToken);
    xhr.timeout=180000;
    if(xhr.upload)xhr.upload.onprogress=function(e){
      if(e.lengthComputable)onProg(Math.round(e.loaded/e.total*100),e.loaded,e.total);
    };
    xhr.onload=function(){
      if(xhr.status===401)on401();
      var d=null;try{d=JSON.parse(xhr.responseText);}catch(e){}
      if(xhr.status===200&&d&&d.success)resolve({ok:true});
      else resolve({ok:false,err:String((d&&d.error)||('HTTP '+xhr.status))});
    };
    xhr.onerror=function(){resolve({ok:false,err:'unreachable'});};
    xhr.ontimeout=function(){resolve({ok:false,err:'timed out'});};
    var fd=new FormData();
    fd.append('firmware',file,file.name||'firmware.bin');
    xhr.send(fd);
  });
}
function fleetBtns(disabled){
  ['fleetOtaBtn','fleetScanBtn','fleetRestartBtn'].forEach(function(id){
    var b=el(id);if(b)b.disabled=disabled;
  });
}
async function fleetUpdateRun(file){
  otaBusy=true;
  fleetBtns(true);
  var btn=el('fleetOtaBtn');if(btn)btn.textContent='Updating…';
  var panel=el('fleetOtaPanel');panel.style.display='block';panel.innerHTML='';
  var selfH=fleet.filter(isSelfHost)[0]||null;
  var peers=fleet.filter(function(h){return !isSelfHost(h);});
  var ok=0,fail=0,skip=0;
  var selfChip=String((lastStatus&&lastStatus.chip)||'');
  // A peer that reported ota:false has a single-slot partition table (needs a
  // USB flash), and a different chip family can never run this image — don't
  // waste a 2MB upload on either. Peers predating these fields are attempted.
  function otaSkipReason(h){
    var pd=fleetData[h]&&fleetData[h].d;
    if(!pd)return null;
    if(pd.ota===false)return 'no OTA partition (USB flash needed)';
    if(pd.chip&&selfChip&&pd.chip!==selfChip)return 'different chip ('+pd.chip+' vs '+selfChip+')';
    return null;
  }
  // Sequential on purpose: flashing is the slow part on-device, and parallel
  // uploads would just contend for WiFi airtime and browser sockets.
  for(var i=0;i<peers.length;i++){
    var st=otaRow(panel,peers[i]);
    var why=otaSkipReason(peers[i]);
    if(why){skip++;st.textContent='skipped — '+why;st.className='log-msg ev-warn';continue;}
    var res=await otaUpload('http://'+peers[i]+'/api/ota',file,function(p){st.textContent='uploading… '+p+'%';});
    if(res.ok){ok++;st.textContent='updated — restarting';st.className='log-msg ev-ok';}
    else{fail++;st.textContent='failed: '+res.err;st.className='log-msg ev-err';}
  }
  if(peers.length)pushLog('Fleet update: '+ok+' of '+peers.length+' peer'+(peers.length===1?'':'s')+' updated'+(fail?(', '+fail+' failed'):'')+(skip?(', '+skip+' skipped'):''),fail?'warn':'ok');
  if(selfH&&lastStatus&&lastStatus.ota===false){
    var stS=otaRow(panel,selfH+' (this device)');
    stS.textContent='skipped — no OTA partition (USB flash needed)';
    stS.className='log-msg ev-warn';
    selfH=null;skip++;
  }
  if(selfH){
    var st2=otaRow(panel,selfH+' (this device)');
    var res2=await otaUpload('/api/ota',file,function(p){st2.textContent='uploading… '+p+'%';});
    if(res2.ok){
      st2.textContent='flashed — restarting, page reloads when it\'s back…';
      st2.className='log-msg ev-ok';
      pushLog('This device flashed — restarting','ok');
      var tries=0;
      var t=setInterval(function(){
        tries++;
        if(tries>40){
          clearInterval(t);
          st2.textContent='flashed — restarted, reload the page manually';
          // un-wedge the dashboard: device may be back on a new IP
          otaBusy=false;
          fleetBtns(false);
          if(btn)btn.textContent='Update all';
          return;
        }
        api('/api/status',{cache:'no-store'})
          .then(function(r){if(r.ok){clearInterval(t);location.reload();}})
          .catch(function(){});
      },3000);
      return; // keep otaBusy while polling: this page is done, a fresh one takes over
    }
    fail++;st2.textContent='failed: '+res2.err;st2.className='log-msg ev-err';
    pushLog('Self update failed: '+res2.err,'err');
  }
  if(fail)toast('Fleet update finished with '+fail+' failure'+(fail===1?'':'s'),'warn');
  else toast('Fleet update complete — miners restarting','ok');
  otaBusy=false;
  fleetBtns(false);
  if(btn)btn.textContent='Update all';
  fleetRefresh();
}
window.fleetUpdateAll=function(){
  if(otaBusy)return;
  if(!fleet.length){toast('No miners in fleet','warn');return;}
  el('fleetOtaFile').click();
};
window.fleetOtaPicked=function(){
  var inp=el('fleetOtaFile');
  var f=inp.files&&inp.files[0];
  inp.value='';
  if(!f)return;
  if(!fwValid(f))return;
  if(!confirm('Flash "'+f.name+'" ('+fmtB(f.size)+') to ALL '+fleet.length+' miner'+(fleet.length===1?'':'s')+'?\n\nEach miner restarts after flashing. This device is flashed last.'))return;
  fleetUpdateRun(f);
};

function pushAlert(type,msg){
  alertLog.unshift({t:nowT(),type:type,msg:msg});
  if(alertLog.length>30)alertLog.pop();
  renderAlerts();
}
function renderAlerts(){
  var c=el('alertList');if(!c)return;
  set('alertCount',alertLog.length+' event'+(alertLog.length===1?'':'s'));
  if(!alertLog.length){c.innerHTML='<div class="empty-state">No alerts -- all systems operational</div>';return;}
  c.innerHTML=alertLog.map(function(a){
    var cls=a.type==='err'?'ev-err':a.type==='warn'?'ev-warn':a.type==='ok'?'ev-ok':'ev-inf';
    return '<div class="log-row"><span class="log-ts">'+a.t+'</span><span class="log-msg '+cls+'">'+a.msg+'</span></div>';
  }).join('');
}

function pushLog(msg,type){
  sysLog.unshift({t:nowT(),msg:msg,type:type||'inf'});
  if(sysLog.length>50)sysLog.pop();
  renderLogs();
}
function renderLogs(){
  var c=el('logList');if(!c)return;
  set('logCount',sysLog.length+' entr'+(sysLog.length===1?'y':'ies'));
  if(!sysLog.length){c.innerHTML='<div class="empty-state">No log entries yet</div>';return;}
  c.innerHTML=sysLog.map(function(l){
    var cls=l.type==='err'?'ev-err':l.type==='warn'?'ev-warn':l.type==='ok'?'ev-ok':'ev-inf';
    return '<div class="log-row"><span class="log-ts">'+l.t+'</span><span class="log-msg '+cls+'">'+l.msg+'</span></div>';
  }).join('');
}

function applyStatus(d){
  var khs=d.hashrate_khs||0;
  hrBuf.push(khs);if(hrBuf.length>MAX_HR)hrBuf.shift();
  renderChart();
  set('scH',fmtH(khs));
  set('scHs',khs.toFixed(2)+' KH/s  |  '+fmtN(d.total_mhashes)+' MH total');
  set('scA',fmtN(d.shares));
  set('scAs',fmtN(d.valids)+' block solution'+(d.valids===1?'':'s'));
  if(d.ev_share_accepted){lastShare=nowT();addShare('accepted',d.best_diff);pushLog('Share accepted -- diff: '+fmtD(d.best_diff),'ok');}
  if(d.ev_share_rejected){rejected++;set('scR',String(rejected));addShare('rejected',d.best_diff);pushAlert('err','Share rejected (best diff: '+fmtD(d.best_diff)+')');pushLog('Share rejected','err');}
  if(d.ev_pool_disconnect){pushAlert('warn','Pool disconnected');pushLog('Pool disconnected','warn');}
  set('scU',fmtUp(d.uptime||0));
  set('scUs',d.uptime_str||'');
  var fh=d.free_heap||0,th=d.total_heap||1;
  set('scF',fmtB(fh));
  set('scFs',Math.round(fh/th*100)+'% free of '+fmtB(th));
  set('scT',fmtN(d.templates));
  var fw=(d.firmware||'').replace(/^v/,'');
  set('verBadge','v'+(fw||'--'));
  set('otaV',d.firmware||'--');
  set('sIp',d.ip||'--');
  var ok=d.pool_connected&&d.pool_subscribed;
  var badge=el('connBadge');
  if(badge){badge.className='badge '+(ok?'b-ok':'b-err');set('connTxt',ok?'Connected':d.pool_connected?'Syncing':'Disconnected');}
  if(prevConn!==null&&prevConn!==ok){
    if(ok){pushAlert('ok','Pool connected');pushLog('Pool connected','ok');}
  }
  prevConn=ok;
  var hd=el('healthDot');if(hd)hd.style.background=ok?'var(--grn)':'var(--red)';
  set('healthTxt',ok?'Healthy':'Degraded');
  // VPN badge: green when the tunnel is up, muted when enabled-but-down
  var vb=el('vpnBadge');
  if(vb){
    if(d.wg_enabled){vb.style.display='';vb.className='badge '+(d.wg_connected?'b-ok':'b-err');vb.title='WireGuard VPN '+(d.wg_connected?'tunnel up':'enabled — connecting');}
    else vb.style.display='none';
  }
  var pu=d.pool_url||'';
  set('ppName',pu||'--');
  set('ppUrl',pu+(d.pool_port?':'+d.pool_port:''));
  var ppSt=el('ppSt');if(ppSt){ppSt.textContent=ok?'Connected':'Disconnected';ppSt.className='pr-v '+(ok?'vg':'vr');}
  set('ppLs',lastShare||'--');
  set('ppDf',fmtD(d.best_diff));
  set('ppWk',trunc(d.wallet,20));
  set('ppWa',trunc(d.wallet,12));
  var bd=parseFloat(d.best_diff)||0;
  if(bd>0){diffHist.push(bd);if(diffHist.length>7)diffHist.shift();}
  renderDbars();
  var sv=(d.pool_url||'')+':'+(d.pool_port||'');
  activePool=sv;
  var opt=el('poolSel').querySelector('option[value="'+sv+'"]');
  if(opt)el('poolSel').value=sv;
  // Fleet self-tracking: remember our own data and auto-enrol this device
  lastWallet=d.wallet||lastWallet;
  selfIp=d.ip||selfIp;
  lastStatus=d;
  if(d.ip&&d.ip!=='0.0.0.0'&&!selfAdded){
    selfAdded=true;
    if(!fleetLoaded){
      fleetPendingSelf=d.ip; // applied once fleetLoad() reconciles with the device's list
    }else if(fleet.indexOf(d.ip)<0){
      fleet.push(d.ip);fleetCache();fleetMutate([d.ip],[]);
      if(currentView==='fleet')renderFleet();
    }
  }
  lastTs=new Date();updateFooter();
}

function updateFooter(){
  if(!lastTs)return;
  var s=Math.round((Date.now()-lastTs.getTime())/1000);
  set('ftL','Last updated '+s+'s ago');
  set('ftC','Timezone UTC'+(tz>=0?'+':'')+tz+' · Auto-refresh 3s');
  set('ftR','Save stats to flash (NVS): '+(saveStats?'On':'Off'));
}

function addShare(result,diff){
  shareLog.unshift({t:nowT(),r:result,d:fmtD(diff)});
  if(shareLog.length>8)shareLog.pop();
  var tb=el('sTbody');if(!tb)return;
  tb.innerHTML=shareLog.map(function(s){
    return '<tr><td>'+s.t+'</td><td><span class="'+(s.r==='accepted'?'pa':'pj')+'">'+s.r+'</span></td><td>'+s.d+'</td></tr>';
  }).join('');
}

function applySystem(d){
  set('sChip',d.chip_model||'--');
  set('sCpu',(d.cpu_freq_mhz||'--')+' MHz');
  set('sFlash',fmtB(d.flash_size||0));
  set('sMac',d.mac||'--');
  set('sSdk',(d.sdk_version||'--').substring(0,18));
  var used=(d.total_heap||1)-(d.free_heap||0);
  var pct=Math.round(used/(d.total_heap||1)*100);
  set('sMem',fmtB(used)+' / '+fmtB(d.total_heap||0)+' ('+pct+'% used)');
  var b=el('sMemB');if(b)b.style.width=pct+'%';
}

async function fetchStatus(){
  try{var r=await api('/api/status');if(!r.ok)throw new Error(r.status);applyStatus(await r.json());}
  catch(e){var b=el('connBadge');if(b)b.className='badge b-err';set('connTxt','Offline');}
}
async function fetchSystem(){
  try{var r=await api('/api/system');if(!r.ok)return;applySystem(await r.json());}catch(e){}
}

async function loadCfg(){
  el('cToken').value=apiToken; // prefill even when the config fetch 401s
  try{
    var r=await api('/api/config');if(!r.ok)return;
    var d=await r.json();
    el('cWallet').value=d.wallet||'';el('cUrl').value=d.pool_url||'';
    el('cPort').value=d.pool_port||'';el('cPass').value=d.pool_pass||'';
    el('cTz').value=d.timezone||0;el('cSave').checked=!!d.save_stats;
    tz=d.timezone||0;saveStats=!!d.save_stats;
    // WireGuard section — only shown when the firmware reports the fields
    if('wg_enabled' in d){
      el('wgSection').style.display='block';
      el('cWgEn').checked=!!d.wg_enabled;
      el('cWgIp').value=d.wg_local_ip||'';
      el('cWgEp').value=d.wg_endpoint||'';
      el('cWgPort').value=d.wg_port||51820;
      el('cWgPub').value=d.wg_peer_public_key||'';
      el('cWgPriv').value='';
      set('cWgPkState',d.wg_has_privkey?'(one is set — blank keeps it)':'(none set yet)');
    }else{
      el('wgSection').style.display='none';
    }
  }catch(e){}
}
async function saveCfg(){
  var body={wallet:el('cWallet').value.trim(),pool_url:el('cUrl').value.trim(),pool_port:parseInt(el('cPort').value)||21496,pool_pass:el('cPass').value.trim(),timezone:parseInt(el('cTz').value)||0,save_stats:el('cSave').checked};
  if(!body.wallet){toast('Wallet address required','err');return;}
  if(!body.pool_url){toast('Pool URL required','err');return;}
  // WireGuard fields, only when the section is present (firmware supports it)
  if(el('wgSection').style.display!=='none'){
    var wgEn=el('cWgEn').checked;
    var wgIp=el('cWgIp').value.trim(),wgEp=el('cWgEp').value.trim(),wgPub=el('cWgPub').value.trim(),wgPriv=el('cWgPriv').value.trim();
    if(wgEn&&(!wgIp||!wgEp||!wgPub)){toast('VPN needs tunnel IP, endpoint and server public key','err');return;}
    if(wgEn&&!/^\d+\.\d+\.\d+\.\d+$/.test(wgIp)){toast('Tunnel IP must be an IPv4 address','err');return;}
    body.wg_enabled=wgEn;
    body.wg_local_ip=wgIp;body.wg_endpoint=wgEp;
    body.wg_port=parseInt(el('cWgPort').value)||51820;
    body.wg_peer_public_key=wgPub;
    if(wgPriv)body.wg_private_key=wgPriv; // blank = keep existing key
  }
  set('cMsg','Saving...');
  try{
    var r=await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
    var d=await r.json();
    if(d.success){toast('Saved -- restarting...','ok');set('cMsg','Saved. Device restarting...');}
    else{toast('Error: '+(d.error||'unknown'),'err');set('cMsg','Error: '+(d.error||''));}
  }catch(e){toast('Error: '+e.message,'err');}
}
window.saveCfg=saveCfg;

async function restartDevice(){
  if(!confirm('Restart the miner now?'))return;
  try{await api('/api/restart',{method:'POST'});toast('Restarting...','warn');}catch(e){toast('Restart sent','warn');}
}
window.restartDevice=restartDevice;

async function factoryReset(){
  if(!confirm('Factory reset will erase ALL settings. Are you sure?'))return;
  if(!confirm('This cannot be undone. Confirm?'))return;
  try{await api('/api/reset',{method:'POST'});toast('Factory reset -- restarting...','warn');}catch(e){toast('Reset sent','warn');}
}
window.factoryReset=factoryReset;

async function testPool(){
  toast('Testing connection...','ok');
  try{var r=await api('/api/pool/test');var d=await r.json();toast(d.success?'Pool reachable':'Pool unreachable: '+d.message,d.success?'ok':'err');}
  catch(e){toast('Test failed: '+e.message,'err');}
}
window.testPool=testPool;

async function startOta(){
  var fi=el('otaFile');
  if(!fi.files.length){toast('Select a .bin file first','err');return;}
  var file=fi.files[0];
  if(!fwValid(file))return;
  if(!confirm('Flash '+file.name+' ('+fmtB(file.size)+')?'))return;
  var w=el('otaW'),fill=el('otaFill'),msg=el('otaMsg');
  w.style.display='block';
  var res=await otaUpload('/api/ota',file,function(p,loaded,total){
    fill.style.width=p+'%';
    msg.textContent='Uploading... '+fmtB(loaded)+' / '+fmtB(total);
  });
  if(res.ok){msg.textContent='Flash complete! Restarting...';toast('OTA complete','ok');}
  else{msg.textContent='Failed: '+res.err;toast('OTA failed: '+res.err,'err');}
}
window.startOta=startOta;

function openCfg(){loadCfg();set('cMsg','');el('moCfg').classList.add('show');}
function openOta(){
  if(lastStatus&&lastStatus.ota===false){toast('This board has no OTA partition — flash it over USB','err');return;}
  el('moOta').classList.add('show');
}
function closeM(id){el(id).classList.remove('show');}
window.openCfg=openCfg;window.openOta=openOta;window.closeM=closeM;

document.querySelectorAll('.mo').forEach(function(m){
  m.addEventListener('click',function(e){if(e.target===m)m.classList.remove('show');});
});

// Delegated remove-button handler: reads the host from a data attribute instead
// of an inline onclick, so host strings never reach a JS-string context.
el('fleetBody').addEventListener('click',function(e){
  var b=e.target.closest?e.target.closest('button[data-host]'):null;
  if(b)fleetRemove(b.getAttribute('data-host'));
});

pushLog('Dashboard connected','ok');
fleetLoad();
fetchStatus();fetchSystem();loadCfg();loadPools();
setInterval(fetchStatus,POLL);setInterval(fetchSystem,30000);setInterval(updateFooter,1000);
setInterval(function(){if(currentView==='fleet')fleetRefresh();},5000);
window.addEventListener('resize',renderChart);

})();
</script>
</body>
</html>)rawdash";
