#pragma once
#include <pgmspace.h>

// Dashboard HTML served from PROGMEM — raw string literal, no escaping needed.
// Keep under 30KB; ESP32 reads PROGMEM directly (memory-mapped flash).
const char DASHBOARD_HTML[] PROGMEM = R"rawdash(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>NerdMiner Dashboard</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
:root{
  --bg:#07070f;--card:#0d0d1b;--border:#1c1c3a;
  --accent:#00ffaa;--accent2:#7070ff;--accent3:#ff9500;
  --text:#c0cce0;--muted:#5a6a88;--danger:#ff3c5a;
  --success:#00ffaa;--warn:#ffaa00;
  --font:'Courier New',Courier,monospace;
}
body{background:var(--bg);color:var(--text);font-family:var(--font);font-size:13px;min-height:100vh;padding:8px}
a{color:var(--accent);text-decoration:none}

/* ── Header ── */
.hdr{display:flex;align-items:center;justify-content:space-between;padding:10px 14px;
  background:var(--card);border:1px solid var(--border);border-radius:8px;margin-bottom:10px;flex-wrap:wrap;gap:6px}
.hdr-title{font-size:16px;font-weight:bold;color:var(--accent);letter-spacing:1px}
.hdr-sub{color:var(--muted);font-size:11px}
.hdr-right{display:flex;align-items:center;gap:12px;flex-wrap:wrap}
.status-dot{width:9px;height:9px;border-radius:50%;display:inline-block;margin-right:5px}
.dot-green{background:var(--success);box-shadow:0 0 6px var(--success)}
.dot-red{background:var(--danger);box-shadow:0 0 6px var(--danger)}
.dot-amber{background:var(--warn);box-shadow:0 0 6px var(--warn)}
.status-badge{font-size:11px;padding:3px 8px;border-radius:12px;border:1px solid var(--border);display:flex;align-items:center}
.ver-badge{color:var(--muted);font-size:11px}
.refresh-ts{color:var(--muted);font-size:10px}

/* ── Stats grid ── */
.stats-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(120px,1fr));gap:8px;margin-bottom:10px}
.stat-card{background:var(--card);border:1px solid var(--border);border-radius:8px;padding:10px 12px;
  transition:border-color .3s}
.stat-card:hover{border-color:var(--accent2)}
.stat-label{color:var(--muted);font-size:10px;letter-spacing:.5px;text-transform:uppercase;margin-bottom:4px}
.stat-value{font-size:18px;font-weight:bold;color:var(--accent);white-space:nowrap}
.stat-value.sm{font-size:14px}
.stat-sub{color:var(--muted);font-size:10px;margin-top:2px}

/* ── Chart ── */
.chart-card{background:var(--card);border:1px solid var(--border);border-radius:8px;padding:10px 12px;margin-bottom:10px}
.chart-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:6px}
.chart-label{color:var(--muted);font-size:10px;text-transform:uppercase;letter-spacing:.5px}
.chart-cur{color:var(--accent);font-size:13px}
canvas{width:100%;height:70px;display:block;border-radius:4px}

/* ── Row 2: Pool + WiFi ── */
.info-row{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:10px}
@media(max-width:500px){.info-row{grid-template-columns:1fr}}
.info-card{background:var(--card);border:1px solid var(--border);border-radius:8px;padding:10px 12px}
.info-title{color:var(--muted);font-size:10px;text-transform:uppercase;letter-spacing:.5px;margin-bottom:6px;
  display:flex;align-items:center;gap:6px}
.info-line{display:flex;justify-content:space-between;align-items:center;margin-bottom:4px;font-size:12px}
.info-key{color:var(--muted)}
.info-val{color:var(--text);text-align:right;word-break:break-all;max-width:60%}
.signal-bar{display:inline-flex;align-items:flex-end;gap:2px;height:12px;vertical-align:middle;margin-left:4px}
.signal-bar span{background:var(--muted);border-radius:1px;width:3px}
.signal-bar span.active{background:var(--accent)}

/* ── Collapsible panels ── */
.panel{background:var(--card);border:1px solid var(--border);border-radius:8px;margin-bottom:8px;overflow:hidden}
.panel-hdr{display:flex;align-items:center;justify-content:space-between;padding:10px 14px;cursor:pointer;
  user-select:none;transition:background .2s}
.panel-hdr:hover{background:#111122}
.panel-hdr-title{font-size:12px;color:var(--text);letter-spacing:.5px;text-transform:uppercase;display:flex;align-items:center;gap:8px}
.panel-arrow{color:var(--muted);transition:transform .25s;font-size:10px}
.panel-arrow.open{transform:rotate(90deg)}
.panel-body{padding:12px 14px;border-top:1px solid var(--border);display:none}
.panel-body.open{display:block}

/* ── Forms ── */
.form-row{display:flex;align-items:center;gap:8px;margin-bottom:8px;flex-wrap:wrap}
.form-label{color:var(--muted);font-size:11px;width:110px;flex-shrink:0}
.form-input{flex:1;min-width:120px;background:#060610;border:1px solid var(--border);
  color:var(--text);font-family:var(--font);font-size:12px;padding:5px 8px;border-radius:4px;
  outline:none;transition:border-color .2s}
.form-input:focus{border-color:var(--accent2)}
.form-input[type=number]{max-width:90px}
.form-check{display:flex;align-items:center;gap:8px;margin-bottom:8px}
.form-check input{accent-color:var(--accent);width:14px;height:14px;cursor:pointer}
.form-check label{font-size:12px;color:var(--text);cursor:pointer}
.btn-row{display:flex;gap:8px;flex-wrap:wrap;margin-top:12px}
.btn{padding:6px 14px;border-radius:5px;border:none;cursor:pointer;font-family:var(--font);
  font-size:12px;font-weight:bold;letter-spacing:.5px;transition:opacity .2s,box-shadow .2s}
.btn:hover{opacity:.85}
.btn-primary{background:var(--accent);color:#000}
.btn-primary:hover{box-shadow:0 0 10px var(--accent)}
.btn-secondary{background:transparent;color:var(--accent2);border:1px solid var(--accent2)}
.btn-danger{background:transparent;color:var(--danger);border:1px solid var(--danger)}
.btn-warn{background:transparent;color:var(--warn);border:1px solid var(--warn)}

/* ── OTA progress ── */
.ota-progress{margin-top:10px;display:none}
.progress-track{background:#060610;border:1px solid var(--border);border-radius:4px;height:18px;overflow:hidden}
.progress-fill{background:var(--accent2);height:100%;width:0%;transition:width .3s;border-radius:4px;
  display:flex;align-items:center;justify-content:center;font-size:10px;color:#fff}
.ota-msg{margin-top:6px;font-size:11px;color:var(--muted)}

/* ── Alert toast ── */
.toast{position:fixed;top:12px;right:12px;padding:10px 16px;border-radius:6px;font-size:12px;
  z-index:999;opacity:0;transition:opacity .3s;pointer-events:none;max-width:260px}
.toast.show{opacity:1}
.toast-ok{background:#0a2a1a;border:1px solid var(--success);color:var(--success)}
.toast-err{background:#2a0a0f;border:1px solid var(--danger);color:var(--danger)}
.toast-warn{background:#2a1a00;border:1px solid var(--warn);color:var(--warn)}

/* ── Footer ── */
.footer{display:flex;justify-content:space-between;align-items:center;padding:8px 0;
  color:var(--muted);font-size:10px;flex-wrap:wrap;gap:6px}
.footer-actions{display:flex;gap:8px}
</style>
</head>
<body>

<!-- Toast -->
<div id="toast" class="toast"></div>

<!-- Header -->
<div class="hdr">
  <div>
    <div class="hdr-title">⚡ NerdMiner Dashboard</div>
    <div class="hdr-sub" id="hostname">Loading…</div>
  </div>
  <div class="hdr-right">
    <span class="status-badge" id="statusBadge">
      <span class="status-dot dot-amber" id="statusDot"></span>
      <span id="statusText">Connecting</span>
    </span>
    <span class="ver-badge" id="verBadge">—</span>
    <span class="refresh-ts" id="lastUpdate">—</span>
  </div>
</div>

<!-- Stats Row -->
<div class="stats-grid">
  <div class="stat-card">
    <div class="stat-label">Hashrate</div>
    <div class="stat-value" id="hashrate">—</div>
    <div class="stat-sub" id="hashrateKhs">0 KH/s</div>
  </div>
  <div class="stat-card">
    <div class="stat-label">Shares</div>
    <div class="stat-value" id="shares">—</div>
    <div class="stat-sub" id="valids">0 valid blocks</div>
  </div>
  <div class="stat-card">
    <div class="stat-label">Best Diff</div>
    <div class="stat-value sm" id="bestDiff">—</div>
    <div class="stat-sub">all-time best</div>
  </div>
  <div class="stat-card">
    <div class="stat-label">Uptime</div>
    <div class="stat-value sm" id="uptime">—</div>
    <div class="stat-sub" id="uptimeSub">—</div>
  </div>
  <div class="stat-card">
    <div class="stat-label">Free Heap</div>
    <div class="stat-value sm" id="freeHeap">—</div>
    <div class="stat-sub" id="heapPct">of total</div>
  </div>
  <div class="stat-card">
    <div class="stat-label">Templates</div>
    <div class="stat-value" id="templates">—</div>
    <div class="stat-sub">jobs received</div>
  </div>
</div>

<!-- Hashrate Chart -->
<div class="chart-card">
  <div class="chart-header">
    <span class="chart-label">Hashrate — Last 90s</span>
    <span class="chart-cur" id="chartCur">—</span>
  </div>
  <canvas id="hrChart"></canvas>
</div>

<!-- Pool + WiFi -->
<div class="info-row">
  <div class="info-card">
    <div class="info-title">
      <span class="status-dot" id="poolDot"></span>Pool
    </div>
    <div class="info-line"><span class="info-key">Host</span><span class="info-val" id="poolHost">—</span></div>
    <div class="info-line"><span class="info-key">Port</span><span class="info-val" id="poolPort">—</span></div>
    <div class="info-line"><span class="info-key">Status</span><span class="info-val" id="poolStatus">—</span></div>
    <div class="info-line"><span class="info-key">Wallet</span><span class="info-val" id="poolWallet">—</span></div>
  </div>
  <div class="info-card">
    <div class="info-title">
      <span class="status-dot dot-green" id="wifiDot"></span>Network
    </div>
    <div class="info-line"><span class="info-key">SSID</span><span class="info-val" id="wifiSsid">—</span></div>
    <div class="info-line"><span class="info-key">Signal</span>
      <span class="info-val">
        <span id="wifiRssi">—</span> dBm
        <span class="signal-bar" id="signalBar">
          <span style="height:3px"></span>
          <span style="height:5px"></span>
          <span style="height:8px"></span>
          <span style="height:11px"></span>
        </span>
      </span>
    </div>
    <div class="info-line"><span class="info-key">IP</span><span class="info-val" id="localIp">—</span></div>
    <div class="info-line"><span class="info-key">Total Hash</span><span class="info-val" id="totalHash">—</span></div>
  </div>
</div>

<!-- Config Panel -->
<div class="panel" id="cfgPanel">
  <div class="panel-hdr" onclick="togglePanel('cfgPanel')">
    <span class="panel-hdr-title">⚙ Configuration</span>
    <span class="panel-arrow" id="cfgArrow">▶</span>
  </div>
  <div class="panel-body" id="cfgBody">
    <div class="form-row">
      <span class="form-label">Pool URL</span>
      <input class="form-input" id="cfgPoolUrl" type="text" placeholder="public-pool.io">
    </div>
    <div class="form-row">
      <span class="form-label">Pool Port</span>
      <input class="form-input" id="cfgPoolPort" type="number" min="1" max="65535" placeholder="21496">
    </div>
    <div class="form-row">
      <span class="form-label">Wallet Address</span>
      <input class="form-input" id="cfgWallet" type="text" placeholder="bc1q...">
    </div>
    <div class="form-row">
      <span class="form-label">Pool Password</span>
      <input class="form-input" id="cfgPoolPass" type="text" placeholder="x">
    </div>
    <div class="form-row">
      <span class="form-label">Timezone UTC</span>
      <input class="form-input" id="cfgTimezone" type="number" min="-12" max="14" placeholder="0">
    </div>
    <div class="form-check">
      <input type="checkbox" id="cfgSaveStats">
      <label for="cfgSaveStats">Save mining statistics to flash (NVS)</label>
    </div>
    <div class="btn-row">
      <button class="btn btn-primary" onclick="saveConfig()">💾 Save &amp; Restart</button>
      <button class="btn btn-secondary" onclick="loadConfig()">↺ Reload</button>
    </div>
    <div id="cfgMsg" style="margin-top:8px;font-size:11px;color:var(--muted)"></div>
  </div>
</div>

<!-- OTA Panel -->
<div class="panel" id="otaPanel">
  <div class="panel-hdr" onclick="togglePanel('otaPanel')">
    <span class="panel-hdr-title">🔄 Firmware Update (OTA)</span>
    <span class="panel-arrow" id="otaArrow">▶</span>
  </div>
  <div class="panel-body" id="otaBody">
    <p style="color:var(--muted);font-size:11px;margin-bottom:10px">
      Upload a compiled .bin firmware file. The device will restart automatically.
      Current: <span id="otaCurrentVer" style="color:var(--accent)">—</span>
    </p>
    <div class="form-row">
      <input class="form-input" type="file" id="otaFile" accept=".bin" style="flex:1;padding:4px">
      <button class="btn btn-secondary" onclick="startOta()">⬆ Flash</button>
    </div>
    <div class="ota-progress" id="otaProgress">
      <div class="progress-track"><div class="progress-fill" id="otaFill">0%</div></div>
      <div class="ota-msg" id="otaMsg">Uploading…</div>
    </div>
  </div>
</div>

<!-- System Panel -->
<div class="panel" id="sysPanel">
  <div class="panel-hdr" onclick="togglePanel('sysPanel')">
    <span class="panel-hdr-title">🖥 System Info</span>
    <span class="panel-arrow" id="sysPanelArrow">▶</span>
  </div>
  <div class="panel-body" id="sysPanelBody">
    <div class="info-line"><span class="info-key">Firmware</span><span class="info-val" id="sysFw">—</span></div>
    <div class="info-line"><span class="info-key">Hostname</span><span class="info-val" id="sysHost">—</span></div>
    <div class="info-line"><span class="info-key">Total Heap</span><span class="info-val" id="sysTotalHeap">—</span></div>
    <div class="info-line"><span class="info-key">Min Free Heap</span><span class="info-val" id="sysMinHeap">—</span></div>
    <div class="info-line"><span class="info-key">CPU Freq</span><span class="info-val" id="sysCpu">—</span></div>
    <div class="info-line"><span class="info-key">Flash Size</span><span class="info-val" id="sysFlash">—</span></div>
    <div class="info-line"><span class="info-key">MAC Address</span><span class="info-val" id="sysMac">—</span></div>
    <div class="info-line"><span class="info-key">SDK Version</span><span class="info-val" id="sysSdk">—</span></div>
    <div class="info-line"><span class="info-key">Chip Model</span><span class="info-val" id="sysChip">—</span></div>
  </div>
</div>

<!-- Footer -->
<div class="footer">
  <div class="footer-actions">
    <button class="btn btn-warn" onclick="restartDevice()">↺ Restart</button>
    <button class="btn btn-danger" onclick="factoryReset()">⚠ Factory Reset</button>
  </div>
  <div>
    <span id="footerIp">—</span> &nbsp;·&nbsp; NerdMiner Enhanced
  </div>
</div>

<script>
(function(){
'use strict';

// ── Chart ──────────────────────────────────────────────────────────────
const CHART_SAMPLES = 30;
const hrBuf = [];
let hrMax = 0;

function chartPush(val) {
  hrBuf.push(val);
  if (hrBuf.length > CHART_SAMPLES) hrBuf.shift();
  hrMax = Math.max(...hrBuf, 1);
}

function chartDraw() {
  const canvas = document.getElementById('hrChart');
  if (!canvas) return;
  const dpr = window.devicePixelRatio || 1;
  const rect = canvas.parentElement.getBoundingClientRect();
  const w = rect.width - 24;
  const h = 70;
  canvas.width = w * dpr;
  canvas.height = h * dpr;
  canvas.style.width = w + 'px';
  canvas.style.height = h + 'px';
  const ctx = canvas.getContext('2d');
  ctx.scale(dpr, dpr);
  ctx.clearRect(0, 0, w, h);

  if (hrBuf.length < 2) return;

  const step = w / (CHART_SAMPLES - 1);
  const scaleY = (h - 8) / hrMax;
  const startIdx = CHART_SAMPLES - hrBuf.length;

  // Grid lines
  ctx.strokeStyle = '#1c1c3a';
  ctx.lineWidth = 1;
  for (let i = 1; i < 4; i++) {
    const y = Math.round(h * i / 4) + 0.5;
    ctx.beginPath(); ctx.moveTo(0, y); ctx.lineTo(w, y); ctx.stroke();
  }

  // Fill gradient
  const grad = ctx.createLinearGradient(0, 0, 0, h);
  grad.addColorStop(0, 'rgba(0,255,170,0.18)');
  grad.addColorStop(1, 'rgba(0,255,170,0.01)');

  ctx.beginPath();
  hrBuf.forEach((v, i) => {
    const x = (startIdx + i) * step;
    const y = h - 4 - v * scaleY;
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  });
  const lastX = (startIdx + hrBuf.length - 1) * step;
  ctx.lineTo(lastX, h);
  ctx.lineTo(startIdx * step, h);
  ctx.closePath();
  ctx.fillStyle = grad;
  ctx.fill();

  // Line
  ctx.beginPath();
  ctx.strokeStyle = '#00ffaa';
  ctx.lineWidth = 1.5;
  ctx.lineJoin = 'round';
  hrBuf.forEach((v, i) => {
    const x = (startIdx + i) * step;
    const y = h - 4 - v * scaleY;
    if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  });
  ctx.stroke();

  // Latest dot
  const lx = (startIdx + hrBuf.length - 1) * step;
  const ly = h - 4 - hrBuf[hrBuf.length-1] * scaleY;
  ctx.beginPath();
  ctx.arc(lx, ly, 3, 0, Math.PI*2);
  ctx.fillStyle = '#00ffaa';
  ctx.fill();
}

// ── Formatting ─────────────────────────────────────────────────────────
function fmtH(khs) {
  if (khs >= 1000) return (khs/1000).toFixed(2) + ' MH/s';
  if (khs >= 1)    return khs.toFixed(1) + ' KH/s';
  return (khs * 1000).toFixed(0) + ' H/s';
}

function fmtBytes(b) {
  if (b >= 1048576) return (b/1048576).toFixed(1) + ' MB';
  if (b >= 1024)    return (b/1024).toFixed(0) + ' KB';
  return b + ' B';
}

function fmtNum(n) {
  return n ? n.toLocaleString() : '0';
}

function fmtUptime(s) {
  const d = Math.floor(s / 86400);
  const h = Math.floor((s % 86400) / 3600);
  const m = Math.floor((s % 3600) / 60);
  if (d > 0) return d + 'd ' + h + 'h';
  if (h > 0) return h + 'h ' + m + 'm';
  return m + 'm ' + (s%60) + 's';
}

function fmtDiff(d) {
  if (!d) return '0';
  if (d >= 1) return d.toFixed(4);
  return d.toExponential(3);
}

function el(id) { return document.getElementById(id); }
function set(id, v) { const e = el(id); if (e) e.textContent = v; }

// ── Signal bars ─────────────────────────────────────────────────────────
function updateSignal(rssi) {
  const bars = document.querySelectorAll('#signalBar span');
  // rssi thresholds: -50 (4 bars), -65 (3), -75 (2), -85 (1)
  const levels = rssi >= -50 ? 4 : rssi >= -65 ? 3 : rssi >= -75 ? 2 : rssi >= -90 ? 1 : 0;
  bars.forEach((b, i) => {
    b.className = i < levels ? 'active' : '';
  });
}

// ── Toast ───────────────────────────────────────────────────────────────
let toastTimer;
function toast(msg, type) {
  const t = el('toast');
  t.textContent = msg;
  t.className = 'toast toast-' + (type || 'ok') + ' show';
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => { t.classList.remove('show'); }, 3200);
}

// ── Panel toggle ────────────────────────────────────────────────────────
function togglePanel(id) {
  const body = el(id + 'Body') || el(id.replace('Panel','') + 'Body');
  const arrow = el(id + 'Arrow') || el(id.replace('Panel','') + 'Arrow');
  if (!body) return;
  const open = body.classList.toggle('open');
  if (arrow) arrow.className = 'panel-arrow' + (open ? ' open' : '');
}
window.togglePanel = togglePanel;

// ── Status update ────────────────────────────────────────────────────────
function applyStatus(d) {
  const khs = d.hashrate_khs || 0;
  chartPush(khs);
  chartDraw();

  set('hashrate', fmtH(khs));
  set('hashrateKhs', khs.toFixed(2) + ' KH/s raw');
  set('chartCur', fmtH(khs));
  set('shares', fmtNum(d.shares));
  set('valids', (d.valids || 0) + ' valid block' + (d.valids === 1 ? '' : 's'));
  set('bestDiff', fmtDiff(d.best_diff));
  set('uptime', fmtUptime(d.uptime || 0));
  set('uptimeSub', (d.uptime_str || '') );
  set('templates', fmtNum(d.templates));

  const fh = d.free_heap || 0, th = d.total_heap || 1;
  set('freeHeap', fmtBytes(fh));
  set('heapPct', Math.round(fh/th*100) + '% free of ' + fmtBytes(th));

  // Pool
  const poolOk = d.pool_connected && d.pool_subscribed;
  el('poolDot').className = 'status-dot ' + (poolOk ? 'dot-green' : 'dot-red');
  set('poolHost', d.pool_url || '—');
  set('poolPort', d.pool_port || '—');
  set('poolStatus', poolOk ? 'Connected & Subscribed' : (d.pool_connected ? 'Connected' : 'Disconnected'));
  const w = d.wallet || '';
  set('poolWallet', w.length > 16 ? w.slice(0,8)+'…'+w.slice(-8) : w || '—');

  // WiFi
  const rssi = d.wifi_rssi || -100;
  set('wifiSsid', d.wifi_ssid || '—');
  set('wifiRssi', rssi);
  updateSignal(rssi);
  set('localIp', d.ip || '—');
  set('totalHash', (d.total_mhashes || 0).toLocaleString() + ' MH total');

  // Header
  set('hostname', d.hostname || 'NerdMiner');
  set('verBadge', d.firmware || '—');
  set('otaCurrentVer', d.firmware || '—');
  set('footerIp', d.ip || '—');

  // Status badge
  const s = d.status || 'unknown';
  const dotClass = s === 'mining' ? 'dot-green' : s === 'connecting' ? 'dot-amber' : 'dot-red';
  el('statusDot').className = 'status-dot ' + dotClass;
  set('statusText', s.replace('_',' ').toUpperCase());

  // System panel (passive update)
  set('sysFw', d.firmware || '—');
  set('sysHost', d.hostname || '—');

  const now = new Date();
  set('lastUpdate', '↻ ' + now.toLocaleTimeString());
}

// ── Fetch status ─────────────────────────────────────────────────────────
async function fetchStatus() {
  try {
    const r = await fetch('/api/status');
    if (!r.ok) throw new Error(r.status);
    const d = await r.json();
    applyStatus(d);
  } catch(e) {
    set('statusText', 'OFFLINE');
    el('statusDot').className = 'status-dot dot-red';
    set('lastUpdate', '↻ Error: ' + e.message);
  }
}

// ── Fetch system info ────────────────────────────────────────────────────
async function fetchSystem() {
  try {
    const r = await fetch('/api/system');
    if (!r.ok) return;
    const d = await r.json();
    set('sysTotalHeap', fmtBytes(d.total_heap||0));
    set('sysMinHeap', fmtBytes(d.min_free_heap||0));
    set('sysCpu', (d.cpu_freq_mhz||0) + ' MHz');
    set('sysFlash', fmtBytes(d.flash_size||0));
    set('sysMac', d.mac||'—');
    set('sysSdk', d.sdk_version||'—');
    set('sysChip', d.chip_model||'—');
  } catch(_){}
}

// ── Config ───────────────────────────────────────────────────────────────
async function loadConfig() {
  try {
    const r = await fetch('/api/config');
    if (!r.ok) throw new Error(r.status);
    const d = await r.json();
    el('cfgPoolUrl').value  = d.pool_url  || '';
    el('cfgPoolPort').value = d.pool_port || '';
    el('cfgWallet').value   = d.wallet    || '';
    el('cfgPoolPass').value = d.pool_pass || '';
    el('cfgTimezone').value = d.timezone  || 0;
    el('cfgSaveStats').checked = !!d.save_stats;
    set('cfgMsg', 'Config loaded');
  } catch(e) {
    set('cfgMsg', 'Load failed: ' + e.message);
  }
}
window.loadConfig = loadConfig;

async function saveConfig() {
  const body = {
    pool_url:   el('cfgPoolUrl').value.trim(),
    pool_port:  parseInt(el('cfgPoolPort').value) || 21496,
    wallet:     el('cfgWallet').value.trim(),
    pool_pass:  el('cfgPoolPass').value.trim(),
    timezone:   parseInt(el('cfgTimezone').value) || 0,
    save_stats: el('cfgSaveStats').checked
  };
  if (!body.pool_url) { toast('Pool URL required', 'err'); return; }
  if (!body.wallet)   { toast('Wallet address required', 'err'); return; }
  try {
    const r = await fetch('/api/config', {
      method:'POST', headers:{'Content-Type':'application/json'},
      body: JSON.stringify(body)
    });
    const d = await r.json();
    if (d.success) {
      toast('Config saved — restarting…', 'ok');
      set('cfgMsg', 'Saved. Device restarting…');
    } else {
      toast('Save failed: ' + (d.error||'unknown'), 'err');
    }
  } catch(e) {
    toast('Error: ' + e.message, 'err');
  }
}
window.saveConfig = saveConfig;

// ── Restart / Reset ──────────────────────────────────────────────────────
async function restartDevice() {
  if (!confirm('Restart the miner now?')) return;
  try {
    await fetch('/api/restart', {method:'POST'});
    toast('Restarting…', 'warn');
  } catch(_) { toast('Restart sent', 'warn'); }
}
window.restartDevice = restartDevice;

async function factoryReset() {
  if (!confirm('Factory reset will erase all settings. Continue?')) return;
  if (!confirm('Are you sure? This cannot be undone.')) return;
  try {
    await fetch('/api/reset', {method:'POST'});
    toast('Factory reset — restarting…', 'warn');
  } catch(_) { toast('Reset sent', 'warn'); }
}
window.factoryReset = factoryReset;

// ── OTA ──────────────────────────────────────────────────────────────────
async function startOta() {
  const fileInput = el('otaFile');
  if (!fileInput.files.length) { toast('Select a .bin file first', 'err'); return; }
  const file = fileInput.files[0];
  if (!file.name.endsWith('.bin')) { toast('File must be a .bin firmware', 'err'); return; }
  if (!confirm('Flash ' + file.name + ' (' + fmtBytes(file.size) + ')?\nDevice will restart after upload.')) return;

  const prog = el('otaProgress');
  const fill = el('otaFill');
  const msg  = el('otaMsg');
  prog.style.display = 'block';

  const formData = new FormData();
  formData.append('firmware', file, file.name);

  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/api/ota');
  xhr.upload.onprogress = (e) => {
    if (e.lengthComputable) {
      const pct = Math.round(e.loaded / e.total * 100);
      fill.style.width = pct + '%';
      fill.textContent = pct + '%';
      msg.textContent = 'Uploading… ' + fmtBytes(e.loaded) + ' / ' + fmtBytes(e.total);
    }
  };
  xhr.onload = () => {
    try {
      const resp = JSON.parse(xhr.responseText);
      if (resp.success) {
        msg.textContent = 'Flash successful! Restarting…';
        toast('OTA complete — restarting', 'ok');
      } else {
        msg.textContent = 'Flash failed: ' + (resp.error || 'unknown');
        toast('OTA failed', 'err');
      }
    } catch(_) {
      msg.textContent = 'Upload complete, restarting…';
      toast('OTA sent', 'ok');
    }
  };
  xhr.onerror = () => {
    msg.textContent = 'Upload error. Check connection.';
    toast('Upload error', 'err');
  };
  xhr.send(formData);
}
window.startOta = startOta;

// ── Boot ─────────────────────────────────────────────────────────────────
fetchStatus();
loadConfig();
fetchSystem();

const POLL_MS = 3000;
setInterval(fetchStatus, POLL_MS);
setInterval(fetchSystem, 30000);

window.addEventListener('resize', chartDraw);
})();
</script>
</body>
</html>)rawdash";
