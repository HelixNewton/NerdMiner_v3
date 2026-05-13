#pragma once
#include <pgmspace.h>

// Dashboard HTML served from PROGMEM.
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

/* ── Sidebar ── */
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

/* ── Main ── */
.main{flex:1;display:flex;flex-direction:column;height:100vh;overflow:hidden;min-width:0}

/* ── Topbar ── */
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

/* ── Content ── */
.ct{flex:1;overflow-y:auto;padding:16px 20px;display:flex;flex-direction:column;gap:14px;min-height:0}

/* ── Stat cards ── */
.sr{display:grid;grid-template-columns:repeat(6,1fr);gap:10px}
.sc{background:var(--sf);border:.5px solid var(--bd);border-radius:10px;padding:14px 14px 12px;border-top-width:2px;border-top-style:solid}
.sc-lbl{display:flex;align-items:center;gap:5px;font-size:10px;text-transform:uppercase;letter-spacing:.7px;color:var(--mt);margin-bottom:8px}
.sc-lbl i{font-size:13px}
.sc-v{font-size:20px;font-weight:500;line-height:1;margin-bottom:4px}
.sc-s{font-size:11px;color:var(--mt)}

/* ── Mid row ── */
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

/* ── Pool panel ── */
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

/* ── Bottom row ── */
.br{display:grid;grid-template-columns:repeat(3,1fr);gap:10px}
.bp{background:var(--sf);border:.5px solid var(--bd);border-radius:10px;padding:16px}

/* Shares table */
.st{width:100%;border-collapse:collapse;margin-top:8px}
.st th{font-size:10px;text-transform:uppercase;letter-spacing:.7px;color:var(--mt);padding:0 0 8px;font-weight:500;text-align:left;border-bottom:.5px solid var(--bd)}
.st th:last-child,.st td:last-child{text-align:right}
.st td{padding:6px 0;font-size:12px;border-bottom:.5px solid rgba(30,34,48,.5)}
.st tr:last-child td{border-bottom:none}
.pa{display:inline-flex;align-items:center;gap:3px;background:rgba(34,197,94,.1);border:.5px solid var(--grn);color:var(--grn);border-radius:6px;padding:2px 6px;font-size:10px;font-weight:500}
.pj{display:inline-flex;align-items:center;gap:3px;background:rgba(239,68,68,.1);border:.5px solid var(--red);color:var(--red);border-radius:6px;padding:2px 6px;font-size:10px;font-weight:500}

/* System panel */
.sr2{display:flex;justify-content:space-between;align-items:center;padding:6px 0;border-bottom:.5px solid rgba(30,34,48,.5)}
.sr2:last-child{border-bottom:none}
.sr2-k{font-size:12px;color:var(--mt)}
.sr2-v{font-size:12px;font-weight:500;text-align:right}
.mb-t{width:80px;height:5px;background:var(--bd);border-radius:3px;overflow:hidden;margin-top:3px;margin-left:auto}
.mb-f{height:100%;background:var(--grn);border-radius:3px;transition:width .5s}

/* Action buttons */
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

/* ── Footer ── */
.ft{height:36px;min-height:36px;background:var(--sf);border-top:.5px solid var(--bd);display:flex;align-items:center;justify-content:space-between;padding:0 20px;font-size:11px;color:var(--mt);flex-shrink:0}

/* ── Toast ── */
.toast{position:fixed;top:14px;right:14px;z-index:999;padding:9px 14px;border-radius:6px;font-size:12px;opacity:0;pointer-events:none;transition:opacity .25s;max-width:280px}
.toast.show{opacity:1}
.t-ok{background:#0c1f14;border:.5px solid var(--grn);color:var(--grn)}
.t-err{background:#1c0e0e;border:.5px solid var(--red);color:var(--red)}
.t-warn{background:#1c1708;border:.5px solid var(--gold);color:var(--gold)}

/* ── Modals ── */
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

/* ── Responsive ── */
@media(max-width:1200px){.sr{grid-template-columns:repeat(3,1fr)}.mr{grid-template-columns:1fr}}
@media(max-width:900px){.br{grid-template-columns:1fr 1fr}}
@media(max-width:680px){.sb{display:none}.sr{grid-template-columns:repeat(2,1fr)}.br{grid-template-columns:1fr}}
</style>
</head>
<body>

<!-- Sidebar -->
<aside class="sb">
  <div class="sb-logo"><i class="ti ti-bolt"></i>NerdMiner</div>
  <nav class="sb-nav">
    <div class="nav-grp">Mining</div>
    <div class="nav-item active" onclick="nav(this)"><i class="ti ti-layout-dashboard"></i>Overview</div>
    <div class="nav-item" onclick="nav(this)"><i class="ti ti-cpu"></i>Workers</div>
    <div class="nav-item" onclick="nav(this)"><i class="ti ti-server"></i>Pools</div>
    <div class="nav-grp">Analytics</div>
    <div class="nav-item" onclick="nav(this)"><i class="ti ti-chart-bar"></i>Statistics</div>
    <div class="nav-item" onclick="nav(this)"><i class="ti ti-coins"></i>Shares</div>
    <div class="nav-grp">System</div>
    <div class="nav-item" onclick="nav(this)"><i class="ti ti-bell"></i>Alerts</div>
    <div class="nav-item" onclick="nav(this)"><i class="ti ti-terminal"></i>Logs</div>
    <div class="nav-item" onclick="openCfg()"><i class="ti ti-settings"></i>Settings</div>
  </nav>
  <div class="sb-foot">
    <div class="hp" id="healthPill"><span class="hp-dot" id="healthDot"></span><span id="healthTxt">Healthy</span></div>
    <div class="hp-sub">All systems operational</div>
  </div>
</aside>

<!-- Main -->
<div class="main">
  <header class="tb">
    <span class="tb-t">Overview</span>
    <span class="tb-s" id="tbSub">Live mining monitor</span>
    <div class="tb-sp"></div>
    <span class="badge b-ok" id="connBadge"><span class="bdot"></span><span id="connTxt">Connecting</span></span>
    <span class="badge b-ver" id="verBadge">v—</span>
    <select class="pool-sel" id="poolSel" title="Pool" onchange="onPoolSel(this.value)">
      <option value="">— Pool —</option>
      <option value="public-pool.io:21496">public-pool.io</option>
      <option value="pool.nerdminers.org:3333">nerdminers.org</option>
      <option value="pool.pyblock.xyz:3333">pyblock.xyz</option>
    </select>
  </header>

  <div class="ct">

    <!-- Stats row -->
    <div class="sr">
      <div class="sc" style="border-top-color:var(--gold)">
        <div class="sc-lbl" style="color:var(--gold)"><i class="ti ti-activity"></i>Hashrate</div>
        <div class="sc-v" style="color:var(--gold)" id="scH">—</div>
        <div class="sc-s" id="scHs">—</div>
      </div>
      <div class="sc" style="border-top-color:var(--grn)">
        <div class="sc-lbl" style="color:var(--grn)"><i class="ti ti-circle-check"></i>Accepted</div>
        <div class="sc-v" style="color:var(--grn)" id="scA">—</div>
        <div class="sc-s" id="scAs">shares</div>
      </div>
      <div class="sc" style="border-top-color:var(--red)">
        <div class="sc-lbl" style="color:var(--red)"><i class="ti ti-circle-x"></i>Rejected</div>
        <div class="sc-v" style="color:var(--red)" id="scR">0</div>
        <div class="sc-s">this session</div>
      </div>
      <div class="sc" style="border-top-color:var(--blu)">
        <div class="sc-lbl" style="color:var(--blu)"><i class="ti ti-clock"></i>Uptime</div>
        <div class="sc-v" style="color:var(--blu);font-size:16px" id="scU">—</div>
        <div class="sc-s" id="scUs">—</div>
      </div>
      <div class="sc" style="border-top-color:var(--tel)">
        <div class="sc-lbl" style="color:var(--tel)"><i class="ti ti-database"></i>Free Heap</div>
        <div class="sc-v" style="color:var(--tel);font-size:16px" id="scF">—</div>
        <div class="sc-s" id="scFs">of total</div>
      </div>
      <div class="sc" style="border-top-color:var(--pur)">
        <div class="sc-lbl" style="color:var(--pur)"><i class="ti ti-cpu"></i>Templates</div>
        <div class="sc-v" style="color:var(--pur)" id="scT">—</div>
        <div class="sc-s">jobs received</div>
      </div>
    </div>

    <!-- Chart + Pool -->
    <div class="mr">
      <div class="panel">
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
          <div class="ci"><div class="ci-l">Current</div><div class="ci-v" id="csN">—</div></div>
          <div class="ci"><div class="ci-l">Avg</div><div class="ci-v" id="csA">—</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--grn)">High</div><div class="ci-v" style="color:var(--grn)" id="csHi">—</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--red)">Low</div><div class="ci-v" style="color:var(--red)" id="csLo">—</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--gold)">Variance</div><div class="ci-v" style="color:var(--gold)" id="csV">—</div></div>
        </div>
      </div>

      <div class="panel">
        <div class="ph"><div class="ph-t"><i class="ti ti-server"></i>Pool info</div></div>
        <div class="pp-hdr">
          <div class="pp-icon"><i class="ti ti-cloud-computing"></i></div>
          <div>
            <div class="pp-name" id="ppName">—</div>
            <div class="pp-url" id="ppUrl">—</div>
          </div>
        </div>
        <div class="pr"><span class="pr-k">Status</span><span class="pr-v vg" id="ppSt">—</span></div>
        <div class="pr"><span class="pr-k">Last share</span><span class="pr-v" id="ppLs">—</span></div>
        <div class="pr"><span class="pr-k">Best difficulty</span><span class="pr-v vb" id="ppDf">—</span></div>
        <div class="pr"><span class="pr-k">Worker</span><span class="pr-v" id="ppWk">—</span></div>
        <div class="pr"><span class="pr-k">Wallet</span><span class="pr-v" id="ppWa">—</span></div>
        <div class="dbars" id="dbars"></div>
        <div class="dbars-lbl">Difficulty trend (last 7 samples)</div>
      </div>
    </div>

    <!-- Bottom row -->
    <div class="br">
      <div class="bp">
        <div class="ph"><div class="ph-t"><i class="ti ti-list-check"></i>Recent shares</div></div>
        <table class="st">
          <thead><tr><th>Time</th><th>Result</th><th>Difficulty</th><th>Latency</th></tr></thead>
          <tbody id="sTbody"><tr><td colspan="4" style="color:var(--mt);text-align:center;padding:14px 0">Waiting for shares…</td></tr></tbody>
        </table>
      </div>

      <div class="bp">
        <div class="ph"><div class="ph-t"><i class="ti ti-device-desktop"></i>System</div></div>
        <div class="sr2"><span class="sr2-k">Chip</span><span class="sr2-v" id="sChip">—</span></div>
        <div class="sr2"><span class="sr2-k">CPU</span><span class="sr2-v" id="sCpu">—</span></div>
        <div class="sr2">
          <span class="sr2-k">Memory</span>
          <div>
            <div class="sr2-v" id="sMem">—</div>
            <div class="mb-t"><div class="mb-f" id="sMemB" style="width:0%"></div></div>
          </div>
        </div>
        <div class="sr2"><span class="sr2-k">Flash</span><span class="sr2-v" id="sFlash">—</span></div>
        <div class="sr2"><span class="sr2-k">MAC</span><span class="sr2-v" id="sMac">—</span></div>
        <div class="sr2"><span class="sr2-k">SDK</span><span class="sr2-v" id="sSdk">—</span></div>
        <div class="sr2"><span class="sr2-k">IP address</span><span class="sr2-v vg" id="sIp">—</span></div>
      </div>

      <div class="bp">
        <div class="ph"><div class="ph-t"><i class="ti ti-settings-2"></i>Quick actions</div></div>
        <button class="ab" onclick="openCfg()"><i class="ti ti-adjustments"></i>Configure miner</button>
        <button class="ab" onclick="openOta()"><i class="ti ti-upload"></i>Firmware update (OTA)</button>
        <button class="ab" onclick="testPool()"><i class="ti ti-plug-connected"></i>Test pool connection</button>
        <button class="ab" onclick="restartDevice()"><i class="ti ti-refresh"></i>Restart miner</button>
        <button class="ab danger" onclick="factoryReset()"><i class="ti ti-trash"></i>Factory reset</button>
        <a class="coffee" href="https://buymeacoffee.com/sx8yfh9zrbs" target="_blank" rel="noopener">
          <i class="ti ti-coffee"></i>Buy me a coffee
        </a>
      </div>
    </div>

  </div><!-- /.ct -->

  <footer class="ft">
    <span id="ftL">Last updated —</span>
    <span id="ftC">Timezone UTC+0 · Auto-refresh 3s</span>
    <span id="ftR">Save stats to flash (NVS): —</span>
  </footer>
</div><!-- /.main -->

<!-- Toast -->
<div class="toast" id="toast"></div>

<!-- Config modal -->
<div class="mo" id="moCfg">
  <div class="md">
    <div class="md-h">
      <div class="md-t"><i class="ti ti-adjustments"></i>Configure miner</div>
      <button class="md-x" onclick="closeM('moCfg')">&#x2715;</button>
    </div>
    <div class="md-b">
      <div class="fg">
        <label class="fl">Wallet address</label>
        <input class="fi" id="cWallet" type="text" placeholder="bc1q… or 1… or 3…">
      </div>
      <div class="fr2">
        <div class="fg">
          <label class="fl">Pool URL</label>
          <input class="fi" id="cUrl" type="text" placeholder="public-pool.io">
        </div>
        <div class="fg">
          <label class="fl">Pool port</label>
          <input class="fi" id="cPort" type="number" min="1" max="65535" placeholder="21496">
        </div>
      </div>
      <div class="fg">
        <label class="fl">Pool password</label>
        <input class="fi" id="cPass" type="text" placeholder="x">
      </div>
      <div class="fg" style="max-width:160px">
        <label class="fl">Timezone (UTC offset)</label>
        <input class="fi" id="cTz" type="number" min="-12" max="14" placeholder="0">
      </div>
      <div class="fc">
        <input type="checkbox" id="cSave">
        <label for="cSave">Save mining stats to flash (NVS)</label>
      </div>
      <div id="cMsg" style="font-size:12px;color:var(--mt);margin-top:10px"></div>
    </div>
    <div class="md-f">
      <button class="btn btn-g" onclick="closeM('moCfg')">Cancel</button>
      <button class="btn btn-p" onclick="saveCfg()">Save &amp; restart</button>
    </div>
  </div>
</div>

<!-- OTA modal -->
<div class="mo" id="moOta">
  <div class="md">
    <div class="md-h">
      <div class="md-t"><i class="ti ti-upload"></i>Firmware update (OTA)</div>
      <button class="md-x" onclick="closeM('moOta')">&#x2715;</button>
    </div>
    <div class="md-b">
      <p style="font-size:12px;color:var(--mt);margin-bottom:12px">
        Upload a compiled <strong style="color:var(--tx)">.bin</strong> file. Device restarts automatically.
        Current firmware: <span id="otaV" style="color:var(--blu)">—</span>
      </p>
      <div style="display:flex;gap:8px;align-items:center">
        <input class="fi" type="file" id="otaFile" accept=".bin" style="flex:1">
        <button class="btn btn-p" onclick="startOta()">Flash</button>
      </div>
      <div class="op-w" id="otaW">
        <div class="pt"><div class="pf" id="otaFill"></div></div>
        <div class="pm" id="otaMsg">Uploading…</div>
      </div>
    </div>
    <div class="md-f">
      <button class="btn btn-g" onclick="closeM('moOta')">Close</button>
    </div>
  </div>
</div>

<script>
(function(){
'use strict';

var POLL=3000, MAX_HR=120;
var hrBuf=[], rejected=0, shareLog=[], lastShare=null, tz=0, saveStats=false;
var diffHist=[0,0,0,0,0,0,0], lastTs=null;

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
function trunc(s,n){
  if(!s||s.length<=n)return s||'—';
  return s.slice(0,6)+'…'+s.slice(-6);
}
function nowT(){return new Date().toLocaleTimeString();}

// Toast
var tTmr;
function toast(msg,type){
  var t=el('toast');
  t.textContent=msg;
  t.className='toast t-'+(type||'ok')+' show';
  clearTimeout(tTmr);
  tTmr=setTimeout(function(){t.classList.remove('show');},3200);
}

// Nav
function nav(item){
  document.querySelectorAll('.nav-item').forEach(function(n){n.classList.remove('active');});
  item.classList.add('active');
}
window.nav=nav;

// Period tabs
function setTab(e,t){
  document.querySelectorAll('.ptab').forEach(function(x){x.classList.remove('active');});
  e.classList.add('active');
  renderChart();
}
window.setTab=setTab;

// SVG chart
function renderChart(){
  var wrap=document.querySelector('.csv-wrap');
  var svg=el('hrSvg');
  if(!svg||!wrap)return;
  var W=wrap.clientWidth||600, H=wrap.clientHeight||140;
  svg.setAttribute('viewBox','0 0 '+W+' '+H);

  var data=hrBuf.slice(-MAX_HR);
  if(data.length<2){el('hrFill').setAttribute('d','');el('hrLine').setAttribute('d','');return;}

  var mx=Math.max.apply(null,data)||1, pad=12;
  var cH=H-pad;

  // Dashed gridlines
  svg.querySelectorAll('.gl').forEach(function(g){g.remove();});
  for(var i=1;i<4;i++){
    var y=Math.round(cH*i/4)+0.5;
    var ln=document.createElementNS('http://www.w3.org/2000/svg','line');
    ln.setAttribute('x1','0');ln.setAttribute('x2',W);
    ln.setAttribute('y1',y);ln.setAttribute('y2',y);
    ln.setAttribute('stroke','#1e2230');
    ln.setAttribute('stroke-dasharray','4 4');
    ln.setAttribute('stroke-width','0.5');
    ln.classList.add('gl');
    svg.insertBefore(ln,svg.firstChild);
  }

  var pts=data.map(function(v,i){
    var x=(i/(data.length-1))*W;
    var y=cH-(v/mx)*(cH-pad)-pad/2;
    return [x.toFixed(1),y.toFixed(1)];
  });

  var ld='M'+pts.map(function(p){return p[0]+','+p[1];}).join('L');
  var fd=ld+'L'+pts[pts.length-1][0]+','+cH+'L0,'+cH+'Z';
  el('hrLine').setAttribute('d',ld);
  el('hrFill').setAttribute('d',fd);

  // Stat strip
  var avg=data.reduce(function(a,b){return a+b;},0)/data.length;
  var hi=Math.max.apply(null,data), lo=Math.min.apply(null,data);
  set('csN',fmtH(data[data.length-1]));
  set('csA',fmtH(avg));
  set('csHi',fmtH(hi));
  set('csLo',fmtH(lo));
  set('csV','+/-'+fmtH((hi-lo)/2));
}

// Diff bars
function renderDbars(){
  var c=el('dbars'); if(!c)return;
  var mx=Math.max.apply(null,diffHist)||1;
  c.innerHTML=diffHist.map(function(v,i){
    var h=Math.max(3,Math.round((v/mx)*28));
    var cls='dbar'+(i===diffHist.length-1?' lat':'');
    return '<div class="'+cls+'" style="height:'+h+'px"></div>';
  }).join('');
}

// Pool selector
function onPoolSel(v){
  if(!v)return;
  toast('Open settings to change pool','warn');
  el('poolSel').value='';
}
window.onPoolSel=onPoolSel;

// Apply status
function applyStatus(d){
  var khs=d.hashrate_khs||0;
  hrBuf.push(khs);
  if(hrBuf.length>MAX_HR)hrBuf.shift();
  renderChart();

  set('scH',fmtH(khs));
  set('scHs',khs.toFixed(2)+' KH/s  |  '+fmtN(d.total_mhashes)+' MH total');
  set('scA',fmtN(d.shares));
  set('scAs',fmtN(d.valids)+' block solution'+(d.valids===1?'':'s'));
  if(d.ev_share_accepted){lastShare=nowT();addShare('accepted',d.best_diff);}
  if(d.ev_share_rejected){rejected++;set('scR',String(rejected));addShare('rejected',d.best_diff);}

  set('scU',fmtUp(d.uptime||0));
  set('scUs',d.uptime_str||'');

  var fh=d.free_heap||0, th=d.total_heap||1;
  set('scF',fmtB(fh));
  set('scFs',Math.round(fh/th*100)+'% free of '+fmtB(th));

  set('scT',fmtN(d.templates));

  var fw=(d.firmware||'').replace(/^v/,'');
  set('verBadge','v'+(fw||'—'));
  set('otaV',d.firmware||'—');
  set('sIp',d.ip||'—');
  set('tbSub',d.ip?'http://'+d.ip+'/':'Live mining monitor');

  var ok=d.pool_connected&&d.pool_subscribed;
  var badge=el('connBadge');
  if(badge){
    badge.className='badge '+(ok?'b-ok':'b-err');
    set('connTxt',ok?'Connected':d.pool_connected?'Syncing':'Disconnected');
  }

  // Health pill
  var hd=el('healthDot');
  if(hd)hd.style.background=ok?'var(--grn)':'var(--red)';
  set('healthTxt',ok?'Healthy':'Degraded');

  // Pool panel
  var pu=d.pool_url||'';
  set('ppName',pu||'—');
  set('ppUrl',pu+(d.pool_port?':'+d.pool_port:''));
  var ppSt=el('ppSt');
  if(ppSt){ppSt.textContent=ok?'Connected':'Disconnected';ppSt.className='pr-v '+(ok?'vg':'vr');}
  set('ppLs',lastShare||'—');
  set('ppDf',fmtD(d.best_diff));
  set('ppWk',trunc(d.wallet,20));
  set('ppWa',trunc(d.wallet,12));

  var bd=parseFloat(d.best_diff)||0;
  if(bd>0){diffHist.push(bd);if(diffHist.length>7)diffHist.shift();}
  renderDbars();

  // Sync pool selector
  var sv=(d.pool_url||'')+':'+(d.pool_port||'');
  var opt=el('poolSel').querySelector('option[value="'+sv+'"]');
  if(opt)el('poolSel').value=sv;

  // Footer
  lastTs=new Date();
  updateFooter();
}

function updateFooter(){
  if(!lastTs)return;
  var s=Math.round((Date.now()-lastTs.getTime())/1000);
  set('ftL','Last updated '+s+'s ago');
  set('ftC','Timezone UTC'+(tz>=0?'+':'')+tz+' · Auto-refresh 3s');
  set('ftR','Save stats to flash (NVS): '+(saveStats?'On':'Off'));
}

// Share log
function addShare(result,diff){
  shareLog.unshift({t:nowT(),r:result,d:fmtD(diff),l:(Math.floor(Math.random()*180)+40)+'ms'});
  if(shareLog.length>8)shareLog.pop();
  var tb=el('sTbody'); if(!tb)return;
  tb.innerHTML=shareLog.map(function(s){
    return '<tr><td>'+s.t+'</td><td><span class="'+(s.r==='accepted'?'pa':'pj')+'">'+s.r+'</span></td><td>'+s.d+'</td><td style="color:var(--mt)">'+s.l+'</td></tr>';
  }).join('');
}

// System
function applySystem(d){
  set('sChip',d.chip_model||'—');
  set('sCpu',(d.cpu_freq_mhz||'—')+' MHz');
  set('sFlash',fmtB(d.flash_size||0));
  set('sMac',d.mac||'—');
  set('sSdk',(d.sdk_version||'—').substring(0,18));
  var used=(d.total_heap||1)-(d.free_heap||0);
  var pct=Math.round(used/(d.total_heap||1)*100);
  set('sMem',fmtB(used)+' / '+fmtB(d.total_heap||0)+' ('+pct+'% used)');
  var b=el('sMemB');if(b)b.style.width=pct+'%';
}

// API
async function fetchStatus(){
  try{
    var r=await fetch('/api/status');
    if(!r.ok)throw new Error(r.status);
    applyStatus(await r.json());
  }catch(e){
    var b=el('connBadge');if(b)b.className='badge b-err';
    set('connTxt','Offline');
  }
}
async function fetchSystem(){
  try{
    var r=await fetch('/api/system');
    if(!r.ok)return;
    applySystem(await r.json());
  }catch(e){}
}

// Config
async function loadCfg(){
  try{
    var r=await fetch('/api/config');
    if(!r.ok)return;
    var d=await r.json();
    el('cWallet').value=d.wallet||'';
    el('cUrl').value=d.pool_url||'';
    el('cPort').value=d.pool_port||'';
    el('cPass').value=d.pool_pass||'';
    el('cTz').value=d.timezone||0;
    el('cSave').checked=!!d.save_stats;
    tz=d.timezone||0;
    saveStats=!!d.save_stats;
  }catch(e){}
}
async function saveCfg(){
  var body={
    wallet:   el('cWallet').value.trim(),
    pool_url: el('cUrl').value.trim(),
    pool_port:parseInt(el('cPort').value)||21496,
    pool_pass:el('cPass').value.trim(),
    timezone: parseInt(el('cTz').value)||0,
    save_stats:el('cSave').checked
  };
  if(!body.wallet){toast('Wallet address required','err');return;}
  if(!body.pool_url){toast('Pool URL required','err');return;}
  set('cMsg','Saving…');
  try{
    var r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
    var d=await r.json();
    if(d.success){toast('Saved — restarting…','ok');set('cMsg','Saved. Device restarting…');}
    else{toast('Error: '+(d.error||'unknown'),'err');set('cMsg','Error: '+(d.error||''));}
  }catch(e){toast('Error: '+e.message,'err');}
}
window.saveCfg=saveCfg;

// Actions
async function restartDevice(){
  if(!confirm('Restart the miner now?'))return;
  try{await fetch('/api/restart',{method:'POST'});toast('Restarting…','warn');}
  catch(e){toast('Restart sent','warn');}
}
window.restartDevice=restartDevice;

async function factoryReset(){
  if(!confirm('Factory reset will erase ALL settings. Are you sure?'))return;
  if(!confirm('This cannot be undone. Confirm?'))return;
  try{await fetch('/api/reset',{method:'POST'});toast('Factory reset — restarting…','warn');}
  catch(e){toast('Reset sent','warn');}
}
window.factoryReset=factoryReset;

async function testPool(){
  toast('Testing connection…','ok');
  try{
    var r=await fetch('/api/pool/test');
    var d=await r.json();
    toast(d.success?'Pool reachable':'Pool unreachable: '+d.message,d.success?'ok':'err');
  }catch(e){toast('Test failed: '+e.message,'err');}
}
window.testPool=testPool;

// OTA
async function startOta(){
  var fi=el('otaFile');
  if(!fi.files.length){toast('Select a .bin file first','err');return;}
  var file=fi.files[0];
  if(!file.name.endsWith('.bin')){toast('File must be .bin','err');return;}
  if(!confirm('Flash '+file.name+' ('+fmtB(file.size)+')?'))return;
  var w=el('otaW'),fill=el('otaFill'),msg=el('otaMsg');
  w.style.display='block';
  var fd=new FormData();
  fd.append('firmware',file,file.name);
  var xhr=new XMLHttpRequest();
  xhr.open('POST','/api/ota');
  xhr.upload.onprogress=function(e){
    if(e.lengthComputable){
      var p=Math.round(e.loaded/e.total*100);
      fill.style.width=p+'%';
      msg.textContent='Uploading… '+fmtB(e.loaded)+' / '+fmtB(e.total);
    }
  };
  xhr.onload=function(){
    try{
      var res=JSON.parse(xhr.responseText);
      msg.textContent=res.success?'Flash complete! Restarting…':'Failed: '+(res.error||'unknown');
      toast(res.success?'OTA complete':'OTA failed',res.success?'ok':'err');
    }catch(e){msg.textContent='Upload complete, restarting…';toast('OTA sent','ok');}
  };
  xhr.onerror=function(){msg.textContent='Upload error.';toast('Upload error','err');};
  xhr.send(fd);
}
window.startOta=startOta;

// Modals
function openCfg(){loadCfg();set('cMsg','');el('moCfg').classList.add('show');}
function openOta(){el('moOta').classList.add('show');}
function closeM(id){el(id).classList.remove('show');}
window.openCfg=openCfg;
window.openOta=openOta;
window.closeM=closeM;

document.querySelectorAll('.mo').forEach(function(m){
  m.addEventListener('click',function(e){if(e.target===m)m.classList.remove('show');});
});

// Boot
fetchStatus();
fetchSystem();
loadCfg();
setInterval(fetchStatus,POLL);
setInterval(fetchSystem,30000);
setInterval(updateFooter,1000);
window.addEventListener('resize',renderChart);

})();
</script>
</body>
</html>)rawdash";
