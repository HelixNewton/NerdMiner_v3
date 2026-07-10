#pragma once
#include <pgmspace.h>

const char DASHBOARD_HTML[] PROGMEM = R"rawdash(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>NerdMiner</title>
<style>
/* ── Design tokens ──────────────────────────────────────────────────────────
   Default theme "aurora". Every theme redefines only these tokens; no rule
   below this block hardcodes a colour. Kept byte-tight: this ships in PROGMEM. */
:root{
  --bg:#05070d; --bg2:#080b12; --bg3:#0d111a;
  --sf:rgba(15,23,42,.55); --sf2:rgba(15,23,42,.75);
  --bd:rgba(148,163,184,.15); --bd2:rgba(148,163,184,.28);
  --tx:#f8fafc; --tx2:#94a3b8; --mt:#475569;
  --acc:#22d3ee; --blu:#3b82f6; --pur:#8b5cf6;
  --grn:#22c55e; --gold:#facc15; --red:#ef4444;
  --glow:0 0 24px rgba(34,211,238,.18);
  --r:14px; --r2:10px;
  --hdr:72px; --sbw:236px; --sbc:72px;
}
[data-theme="classic"]{
  --bg:#0d0f14;--bg2:#0f1116;--bg3:#111318;
  --sf:rgba(17,19,24,.86);--sf2:rgba(17,19,24,.95);
  --bd:#1e2230;--bd2:#2a3040;--tx:#e2e8f0;--tx2:#94a3b8;--mt:#475569;
  --acc:#f4d03f;--blu:#60a5fa;--pur:#a78bfa;--grn:#22c55e;--gold:#f4d03f;--red:#ef4444;
  --glow:0 0 24px rgba(244,208,63,.14);
}
[data-theme="matrix"]{
  --bg:#000600;--bg2:#02120a;--bg3:#041a0e;
  --sf:rgba(4,26,14,.6);--sf2:rgba(4,26,14,.85);
  --bd:rgba(0,255,120,.16);--bd2:rgba(0,255,120,.3);
  --tx:#d3ffe4;--tx2:#5fbf85;--mt:#2f6b47;
  --acc:#00ff85;--blu:#22d3ee;--pur:#7dffb8;--grn:#00ff85;--gold:#b6ff5e;--red:#ff4d4d;
  --glow:0 0 24px rgba(0,255,133,.2);
}
[data-theme="synthwave"]{
  --bg:#0f0524;--bg2:#160831;--bg3:#1c0b3e;
  --sf:rgba(35,12,72,.6);--sf2:rgba(35,12,72,.85);
  --bd:rgba(255,113,206,.18);--bd2:rgba(255,113,206,.34);
  --tx:#fdf4ff;--tx2:#c4a5e8;--mt:#7a5ca8;
  --acc:#ff71ce;--blu:#01cdfe;--pur:#b967ff;--grn:#05ffa1;--gold:#fffb96;--red:#ff4d6d;
  --glow:0 0 26px rgba(255,113,206,.22);
}
[data-theme="nord"]{
  --bg:#20242c;--bg2:#242933;--bg3:#2e3440;
  --sf:rgba(59,66,82,.6);--sf2:rgba(59,66,82,.86);
  --bd:rgba(216,222,233,.14);--bd2:rgba(216,222,233,.26);
  --tx:#eceff4;--tx2:#a9b1c1;--mt:#6b7488;
  --acc:#88c0d0;--blu:#5e81ac;--pur:#b48ead;--grn:#a3be8c;--gold:#ebcb8b;--red:#bf616a;
  --glow:0 0 24px rgba(136,192,208,.18);
}

*{box-sizing:border-box;margin:0;padding:0}
html,body{height:100%}
body{
  background:var(--bg);color:var(--tx);
  font:14px/1.45 ui-sans-serif,system-ui,-apple-system,"Segoe UI",Roboto,"Helvetica Neue",Arial,sans-serif;
  -webkit-font-smoothing:antialiased;
  display:flex;min-height:100vh;overflow-x:hidden;
}
/* Ambient depth: two soft radial washes, no flat black. */
body::before{
  content:"";position:fixed;inset:0;pointer-events:none;z-index:0;
  background:
    radial-gradient(900px 620px at 14% -8%, color-mix(in srgb,var(--pur) 12%,transparent), transparent 62%),
    radial-gradient(800px 560px at 92% 6%, color-mix(in srgb,var(--acc) 10%,transparent), transparent 60%),
    linear-gradient(180deg,var(--bg2),var(--bg));
}
::-webkit-scrollbar{width:9px;height:9px}
::-webkit-scrollbar-thumb{background:var(--bd2);border-radius:6px}
::-webkit-scrollbar-track{background:transparent}
:focus-visible{outline:2px solid var(--acc);outline-offset:2px;border-radius:6px}
.sr-only{position:absolute;width:1px;height:1px;overflow:hidden;clip:rect(0 0 0 0);white-space:nowrap}
svg.i{width:18px;height:18px;flex:none;stroke:currentColor;fill:none;stroke-width:1.7;stroke-linecap:round;stroke-linejoin:round}
svg.i.sm{width:15px;height:15px}
svg.i.big{width:22px;height:22px}

/* ── Sidebar ─────────────────────────────────────────────────────────────── */
.sb{
  position:fixed;inset:0 auto 0 0;width:var(--sbw);z-index:40;
  display:flex;flex-direction:column;
  background:linear-gradient(180deg,color-mix(in srgb,var(--bg3) 92%,transparent),color-mix(in srgb,var(--bg2) 96%,transparent));
  border-right:1px solid var(--bd);backdrop-filter:blur(14px);
  transition:width .22s cubic-bezier(.4,0,.2,1);
}
body.rail .sb{width:var(--sbc)}
.sb-logo{
  height:var(--hdr);display:flex;align-items:center;gap:10px;padding:0 18px;
  font-weight:700;font-size:17px;letter-spacing:-.2px;flex:none;
  border-bottom:1px solid var(--bd);white-space:nowrap;overflow:hidden;
}
.sb-logo .mark{
  width:30px;height:30px;border-radius:9px;flex:none;display:grid;place-items:center;
  background:linear-gradient(135deg,var(--acc),var(--pur));color:#05070d;box-shadow:var(--glow);
}
body.rail .sb-logo .txt{opacity:0;width:0}
.sb-logo .txt{transition:opacity .15s}
.sb-nav{flex:1;overflow-y:auto;overflow-x:hidden;padding:14px 10px}
.nav-grp{
  font-size:10px;font-weight:700;letter-spacing:.13em;text-transform:uppercase;
  color:var(--mt);padding:16px 10px 7px;white-space:nowrap;
}
body.rail .nav-grp{opacity:0;height:16px;padding:6px 0}
.nav-item{
  position:relative;display:flex;align-items:center;gap:11px;
  padding:9px 11px;margin:2px 0;border-radius:var(--r2);
  color:var(--tx2);cursor:pointer;white-space:nowrap;overflow:hidden;
  border:1px solid transparent;
  transition:background .16s,color .16s,border-color .16s;
}
.nav-item:hover{background:color-mix(in srgb,var(--acc) 7%,transparent);color:var(--tx)}
.nav-item.active{
  color:var(--tx);border-color:var(--bd2);
  background:linear-gradient(90deg,color-mix(in srgb,var(--acc) 15%,transparent),color-mix(in srgb,var(--pur) 7%,transparent));
  box-shadow:inset 0 0 20px color-mix(in srgb,var(--acc) 8%,transparent);
}
.nav-item.active::before{
  content:"";position:absolute;left:0;top:18%;bottom:18%;width:3px;border-radius:0 3px 3px 0;
  background:var(--acc);box-shadow:0 0 10px var(--acc);
}
.nav-item.active svg.i{color:var(--acc)}
.nav-item .lbl{transition:opacity .15s}
body.rail .nav-item .lbl{opacity:0;width:0}
body.rail .nav-item{justify-content:center;padding:10px 0}
.nav-item .pip{margin-left:auto;font-size:10px;font-weight:700;padding:1px 6px;border-radius:20px;
  background:color-mix(in srgb,var(--red) 22%,transparent);color:var(--red)}
body.rail .nav-item .pip{display:none}

.sb-foot{padding:12px;border-top:1px solid var(--bd);flex:none}
.hp{
  display:flex;align-items:center;gap:9px;padding:10px 11px;border-radius:var(--r2);
  background:var(--sf);border:1px solid var(--bd);
}
.hp-dot{width:8px;height:8px;border-radius:50%;background:var(--grn);flex:none;
  box-shadow:0 0 0 0 color-mix(in srgb,var(--grn) 60%,transparent);animation:pulse 2.2s infinite}
@keyframes pulse{
  0%{box-shadow:0 0 0 0 color-mix(in srgb,currentColor 55%,transparent)}
  70%{box-shadow:0 0 0 7px transparent}100%{box-shadow:0 0 0 0 transparent}
}
.hp-txt{font-size:12px;font-weight:600;white-space:nowrap}
.hp-sub{font-size:10px;color:var(--mt);padding:6px 4px 0;white-space:nowrap}
body.rail .hp-txt,body.rail .hp-sub{display:none}
body.rail .hp{justify-content:center;padding:10px 0}

/* ── Shell ───────────────────────────────────────────────────────────────── */
.main{flex:1;margin-left:var(--sbw);min-width:0;position:relative;z-index:1;
  transition:margin-left .22s cubic-bezier(.4,0,.2,1);display:flex;flex-direction:column}
body.rail .main{margin-left:var(--sbc)}

.tb{
  position:sticky;top:0;z-index:30;height:var(--hdr);flex:none;
  display:flex;align-items:center;gap:12px;padding:0 22px;
  background:color-mix(in srgb,var(--bg2) 82%,transparent);
  backdrop-filter:blur(14px);border-bottom:1px solid var(--bd);
}
.icon-btn{
  width:36px;height:36px;border-radius:10px;flex:none;display:grid;place-items:center;
  background:transparent;border:1px solid transparent;color:var(--tx2);cursor:pointer;
  transition:background .16s,color .16s,border-color .16s;
}
.icon-btn:hover{background:var(--sf);border-color:var(--bd);color:var(--tx)}
.icon-btn:active{transform:translateY(1px)}

.tb-host{font-size:15px;font-weight:650;letter-spacing:-.2px;color:var(--tx);white-space:nowrap}
@media(max-width:600px){.tb-host{display:none}}
.sp{flex:1}
.tb-r{display:flex;align-items:center;gap:9px}

.badge{
  display:inline-flex;align-items:center;gap:6px;height:30px;padding:0 11px;
  border-radius:20px;font-size:11px;font-weight:600;white-space:nowrap;
  background:var(--sf);border:1px solid var(--bd);color:var(--tx2);
}
.badge .bdot{width:7px;height:7px;border-radius:50%;background:currentColor;flex:none}
.b-ok{color:var(--grn);border-color:color-mix(in srgb,var(--grn) 34%,transparent);
  background:color-mix(in srgb,var(--grn) 10%,transparent)}
.b-ok .bdot{animation:pulse 2.2s infinite}
.b-err{color:var(--red);border-color:color-mix(in srgb,var(--red) 34%,transparent);
  background:color-mix(in srgb,var(--red) 10%,transparent)}
.b-warn{color:var(--gold);border-color:color-mix(in srgb,var(--gold) 34%,transparent);
  background:color-mix(in srgb,var(--gold) 10%,transparent)}
.b-vpn{color:var(--acc);border-color:color-mix(in srgb,var(--acc) 34%,transparent);
  background:color-mix(in srgb,var(--acc) 10%,transparent)}
.b-ver{font-family:ui-monospace,monospace;font-size:10.5px;color:var(--tx2)}

/* dropdown (theme / admin) */
.dd{position:relative}
.dd-menu{
  position:absolute;right:0;top:calc(100% + 8px);min-width:190px;z-index:60;
  background:var(--sf2);backdrop-filter:blur(16px);
  border:1px solid var(--bd2);border-radius:var(--r);padding:6px;
  box-shadow:0 18px 44px rgba(0,0,0,.5);
  opacity:0;transform:translateY(-6px);pointer-events:none;transition:.16s;
}
.dd.open .dd-menu{opacity:1;transform:none;pointer-events:auto}
.dd-h{font-size:10px;font-weight:700;letter-spacing:.1em;text-transform:uppercase;color:var(--mt);padding:8px 10px 5px}
.dd-i{display:flex;align-items:center;gap:9px;padding:8px 10px;border-radius:8px;
  font-size:12.5px;color:var(--tx2);cursor:pointer;border:none;background:none;width:100%;text-align:left;font-family:inherit}
.dd-i:hover{background:color-mix(in srgb,var(--acc) 9%,transparent);color:var(--tx)}
.dd-i .sw{width:12px;height:12px;border-radius:4px;flex:none;border:1px solid var(--bd2)}
.dd-i .ck{margin-left:auto;color:var(--acc);opacity:0}
.dd-i.on .ck{opacity:1}
.dd-sep{height:1px;background:var(--bd);margin:5px 2px}

.ct{padding:22px;flex:1;max-width:1720px;width:100%}
.ph1{display:flex;align-items:flex-end;gap:14px;flex-wrap:wrap;margin-bottom:20px}
.ph1 h1{font-size:26px;font-weight:700;letter-spacing:-.5px}
.ph1 .sub{color:var(--tx2);font-size:13px;padding-bottom:3px}
.ph1 .sp{min-width:20px}
.ranges{display:flex;gap:2px;padding:3px;background:var(--sf);border:1px solid var(--bd);border-radius:11px}
.rg{
  padding:6px 13px;border-radius:8px;font-size:11.5px;font-weight:600;color:var(--tx2);
  cursor:pointer;border:none;background:none;font-family:inherit;transition:.15s;
}
.rg:hover{color:var(--tx)}
.rg.active{background:color-mix(in srgb,var(--acc) 16%,transparent);color:var(--acc);
  box-shadow:inset 0 0 0 1px color-mix(in srgb,var(--acc) 30%,transparent)}
</style>
</head>
<body>
<script>try{var _t=localStorage.getItem('nm_theme');if(_t)document.documentElement.setAttribute('data-theme',_t);if(localStorage.getItem('nm_rail')==='1')document.body.className='rail';}catch(e){}</script>
<style>
/* ── Card primitive ──────────────────────────────────────────────────────── */
.card{
  position:relative;border-radius:var(--r);padding:16px;
  background:var(--sf);backdrop-filter:blur(12px);
  border:1px solid var(--bd);
  box-shadow:0 1px 0 rgba(255,255,255,.03) inset,0 10px 30px rgba(0,0,0,.28);
  transition:transform .18s cubic-bezier(.4,0,.2,1),border-color .18s,box-shadow .18s;
}
.card:hover{transform:translateY(-2px);border-color:var(--bd2);
  box-shadow:0 1px 0 rgba(255,255,255,.05) inset,0 16px 40px rgba(0,0,0,.4),var(--glow)}
@media(prefers-reduced-motion:reduce){
  .hp-dot,.ldot{animation:none!important}
  .card:hover{transform:none}
}
.ph{display:flex;align-items:center;gap:9px;margin-bottom:14px}
.ph-t{display:flex;align-items:center;gap:9px;font-size:13.5px;font-weight:650;letter-spacing:-.1px}
.ph-t .ic{width:28px;height:28px;border-radius:8px;display:grid;place-items:center;flex:none;
  background:color-mix(in srgb,var(--acc) 13%,transparent);color:var(--acc)}
.ph .sp{flex:1}
.live-tag{display:inline-flex;align-items:center;gap:6px;font-size:10.5px;font-weight:700;color:var(--grn)}
.ldot{width:6px;height:6px;border-radius:50%;background:var(--grn);animation:pulse 2s infinite;color:var(--grn)}

/* ── KPI row ─────────────────────────────────────────────────────────────── */
/* 6 KPI cards: 3x2 on mid screens, 6-across on wide so no row orphans. */
.kpis{display:grid;grid-template-columns:repeat(auto-fit,minmax(184px,1fr));gap:14px;margin-bottom:16px}
@media(min-width:1080px){.kpis{grid-template-columns:repeat(3,minmax(0,1fr))}}
@media(min-width:1400px){.kpis{grid-template-columns:repeat(6,minmax(0,1fr))}}
.kpi{position:relative;overflow:hidden;padding:15px}
.kpi::before{content:"";position:absolute;inset:0 0 auto 0;height:2px;background:var(--k);opacity:.85}
.kpi .kh{display:flex;align-items:center;gap:7px;font-size:10.5px;font-weight:700;
  letter-spacing:.08em;text-transform:uppercase;color:var(--k);margin-bottom:9px}
.kpi .kv{font-size:24px;font-weight:700;letter-spacing:-.7px;line-height:1.1;
  font-variant-numeric:tabular-nums;color:var(--tx)}
.kpi .kv.sm{font-size:18px}
.kpi .kf{display:flex;align-items:center;gap:7px;margin-top:6px;font-size:11px;color:var(--tx2)}
.kpi .delta{display:inline-flex;align-items:center;gap:2px;font-weight:650}
.delta.up{color:var(--grn)}.delta.dn{color:var(--red)}.delta.fl{color:var(--mt)}
.kpi .kst{font-size:10px;color:var(--mt);margin-top:2px}

/* skeleton */
.sk{background:linear-gradient(90deg,var(--bd) 25%,var(--bd2) 37%,var(--bd) 63%);
  background-size:400% 100%;animation:shim 1.4s ease infinite;border-radius:6px;color:transparent!important}
@keyframes shim{0%{background-position:100% 0}100%{background-position:-100% 0}}

/* ── Grid regions ────────────────────────────────────────────────────────── */
.gridA{display:grid;grid-template-columns:minmax(0,2fr) minmax(0,1fr) minmax(0,1fr);gap:14px;margin-bottom:16px}
.gridB{display:grid;grid-template-columns:repeat(auto-fit,minmax(290px,1fr));gap:14px;margin-bottom:16px}
.gridC{display:grid;grid-template-columns:minmax(0,2.2fr) minmax(0,1fr);gap:14px}
@media(max-width:1400px){.gridA{grid-template-columns:minmax(0,1fr) minmax(0,1fr)}
  .gridA .chart-card{grid-column:1/-1}}
@media(max-width:1000px){.gridA,.gridB,.gridC{grid-template-columns:1fr}}

/* ── Chart ───────────────────────────────────────────────────────────────── */
.chart-wrap{position:relative;height:260px;margin:2px -4px 0}
.chart-wrap svg{width:100%;height:100%;display:block;overflow:visible}
.tip{
  position:absolute;pointer-events:none;z-index:5;opacity:0;transform:translate(-50%,-120%);
  background:var(--sf2);backdrop-filter:blur(10px);border:1px solid var(--bd2);
  border-radius:9px;padding:7px 10px;font-size:11.5px;white-space:nowrap;
  box-shadow:0 10px 26px rgba(0,0,0,.5);transition:opacity .12s;
}
.tip .tv{font-weight:700;font-variant-numeric:tabular-nums}
.tip .tt{color:var(--mt);font-size:10px}
.cstrip{display:grid;grid-template-columns:repeat(5,1fr);gap:8px;margin-top:14px;
  padding-top:13px;border-top:1px solid var(--bd)}
.ci{text-align:center}
.ci-l{font-size:9.5px;font-weight:700;letter-spacing:.09em;text-transform:uppercase;color:var(--mt)}
.ci-v{font-size:14px;font-weight:700;margin-top:3px;font-variant-numeric:tabular-nums}
@media(max-width:620px){.cstrip{grid-template-columns:repeat(3,1fr);row-gap:12px}}

/* ── Fleet status / donut / gauge ────────────────────────────────────────── */
.fstat{display:flex;flex-direction:column;gap:12px}
.fs-row{display:flex;align-items:center;gap:11px;padding:9px 11px;border-radius:var(--r2);
  background:color-mix(in srgb,var(--bg3) 55%,transparent);border:1px solid var(--bd)}
.fs-row .ic{width:32px;height:32px;border-radius:9px;display:grid;place-items:center;flex:none}
.fs-v{font-size:19px;font-weight:700;line-height:1;font-variant-numeric:tabular-nums}
.fs-l{font-size:10.5px;color:var(--mt);margin-top:3px}
.fs-d{margin-left:auto;font-size:11px;font-weight:650}

.donut-wrap{display:flex;align-items:center;gap:16px;flex-wrap:wrap}
.donut{position:relative;width:132px;height:132px;flex:none}
.donut .dc{position:absolute;inset:0;display:grid;place-items:center;text-align:center}
.donut .dn{font-size:21px;font-weight:700;line-height:1}
.donut .dl{font-size:9.5px;color:var(--mt);text-transform:uppercase;letter-spacing:.08em}
.lg{flex:1;min-width:150px;display:flex;flex-direction:column;gap:7px}
.lg-i{display:flex;align-items:center;gap:8px;font-size:11.5px}
.lg-sw{width:9px;height:9px;border-radius:3px;flex:none}
.lg-n{color:var(--tx2);overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.lg-v{margin-left:auto;font-weight:650;font-variant-numeric:tabular-nums}

.gauge{position:relative;width:118px;height:118px;flex:none;margin:0 auto}
.gauge .gc{position:absolute;inset:0;display:grid;place-items:center;text-align:center}
.gauge .gn{font-size:23px;font-weight:700;line-height:1}
.gauge .gl{font-size:9.5px;color:var(--mt);margin-top:2px}

/* ── Health tiles ────────────────────────────────────────────────────────── */
.htiles{display:grid;grid-template-columns:repeat(auto-fit,minmax(126px,1fr));gap:11px}
.ht{padding:13px;border-radius:var(--r2);text-align:center;
  background:color-mix(in srgb,var(--bg3) 55%,transparent);border:1px solid var(--bd);
  transition:border-color .16s,transform .16s}
.ht:hover{border-color:var(--bd2);transform:translateY(-1px)}
.ht .ic{width:30px;height:30px;border-radius:9px;margin:0 auto 8px;display:grid;place-items:center}
.ht-v{font-size:16px;font-weight:700;font-variant-numeric:tabular-nums;word-break:break-all}
.ht-v.mono{font-size:11.5px;font-family:ui-monospace,monospace;font-weight:600}
.ht-l{font-size:10px;color:var(--mt);margin-top:3px}
.ht-s{font-size:9.5px;margin-top:4px;font-weight:650}
.mb-t{height:4px;border-radius:3px;background:var(--bd);overflow:hidden;margin-top:8px}
.mb-f{height:100%;border-radius:3px;background:linear-gradient(90deg,var(--acc),var(--pur));
  transition:width .5s cubic-bezier(.4,0,.2,1)}

/* ── Insights ────────────────────────────────────────────────────────────── */
.ins{display:flex;flex-direction:column;gap:10px}
.ins-i{position:relative;padding:12px;border-radius:var(--r2);border:1px solid var(--bd);
  background:linear-gradient(135deg,color-mix(in srgb,var(--c) 12%,transparent),transparent 70%);
  transition:border-color .16s}
.ins-i:hover{border-color:color-mix(in srgb,var(--c) 45%,transparent)}
.ins-h{display:flex;align-items:center;gap:7px;font-size:10.5px;font-weight:700;
  letter-spacing:.05em;text-transform:uppercase;color:var(--c);margin-bottom:5px}
.ins-t{font-size:12.5px;font-weight:600;line-height:1.35}
.ins-d{font-size:11px;color:var(--tx2);margin-top:3px;line-height:1.4}
.ins-m{margin-top:8px;font-size:11px;font-weight:700;color:var(--c)}

/* ── Activity feed ───────────────────────────────────────────────────────── */
.feed{display:flex;flex-direction:column;max-height:330px;overflow-y:auto}
.fe{position:relative;display:flex;gap:11px;padding:10px 2px 10px 0}
.fe::before{content:"";position:absolute;left:15px;top:34px;bottom:-10px;width:1px;background:var(--bd)}
.fe:last-child::before{display:none}
.fe .ic{width:31px;height:31px;border-radius:50%;flex:none;display:grid;place-items:center;z-index:1;
  background:var(--bg3);border:1px solid var(--bd)}
.fe-b{flex:1;min-width:0}
.fe-t{font-size:12.5px;font-weight:600}
.fe-d{font-size:11px;color:var(--mt);margin-top:1px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.fe-a{font-size:10.5px;color:var(--mt);white-space:nowrap;padding-top:2px}
.empty{
  display:flex;flex-direction:column;align-items:center;gap:9px;
  padding:34px 16px;text-align:center;color:var(--mt);font-size:12.5px;
}
.empty .ic{width:44px;height:44px;border-radius:13px;display:grid;place-items:center;
  background:color-mix(in srgb,var(--bd) 60%,transparent);color:var(--mt)}
.empty .et{font-weight:600;color:var(--tx2)}
.empty .es{font-size:11px;max-width:280px;line-height:1.5}

/* ── Tables ──────────────────────────────────────────────────────────────── */
.tw{overflow-x:auto;margin:0 -4px}
table.st{width:100%;border-collapse:collapse;font-size:12.5px;min-width:520px}
table.st th{
  text-align:left;font-size:9.5px;font-weight:700;letter-spacing:.09em;text-transform:uppercase;
  color:var(--mt);padding:0 10px 9px;border-bottom:1px solid var(--bd);white-space:nowrap;
}
table.st td{padding:10px;border-bottom:1px solid color-mix(in srgb,var(--bd) 55%,transparent);white-space:nowrap}
table.st tbody tr{transition:background .14s}
table.st tbody tr:hover{background:color-mix(in srgb,var(--acc) 5%,transparent)}
table.st tbody tr:last-child td{border-bottom:none}
.wk{display:flex;align-items:center;gap:9px}
.wk .av{width:27px;height:27px;border-radius:8px;flex:none;display:grid;place-items:center;
  background:color-mix(in srgb,var(--pur) 16%,transparent);color:var(--pur)}
.pa{color:var(--grn);font-weight:650}
.pj{color:var(--red);font-weight:650}
.vb{color:var(--pur);font-weight:650;font-variant-numeric:tabular-nums}
.vg{color:var(--grn)}.vr{color:var(--red)}
.mono{font-family:ui-monospace,monospace}
.fl-link{color:var(--tx);text-decoration:none;font-weight:600}
.fl-link:hover{color:var(--acc)}
.lnk-x{background:none;border:none;color:var(--mt);cursor:pointer;font-size:14px;padding:3px 6px;border-radius:6px}
.lnk-x:hover{color:var(--red);background:color-mix(in srgb,var(--red) 12%,transparent)}
.bar-mini{height:4px;border-radius:3px;background:var(--bd);overflow:hidden;width:64px;margin-top:4px}
.bar-mini i{display:block;height:100%;border-radius:3px;background:var(--grn)}

/* ── Pool card ───────────────────────────────────────────────────────────── */
.pp-hdr{display:flex;align-items:center;gap:12px;padding-bottom:13px;margin-bottom:5px;border-bottom:1px solid var(--bd)}
.pp-icon{width:42px;height:42px;border-radius:12px;flex:none;display:grid;place-items:center;
  background:linear-gradient(135deg,color-mix(in srgb,var(--blu) 26%,transparent),color-mix(in srgb,var(--pur) 20%,transparent));
  color:var(--tx)}
.pp-name{font-weight:650;font-size:14px;line-height:1.35}
.pp-url{font-size:11px;color:var(--mt);font-family:ui-monospace,monospace;line-height:1.4}
.pr{display:flex;justify-content:space-between;align-items:center;gap:10px;padding:8px 0;font-size:12px}
.pr+.pr{border-top:1px solid color-mix(in srgb,var(--bd) 50%,transparent)}
.pr-k{color:var(--tx2)}
.pr-v{font-weight:600;text-align:right;overflow:hidden;text-overflow:ellipsis}
.dbars{display:flex;align-items:flex-end;gap:4px;height:34px;margin-top:13px}
.dbar{flex:1;border-radius:3px 3px 0 0;background:color-mix(in srgb,var(--pur) 40%,transparent);transition:height .4s}
.dbar.lat{background:linear-gradient(180deg,var(--acc),var(--pur));box-shadow:0 0 9px color-mix(in srgb,var(--acc) 45%,transparent)}
.dbars-lbl{font-size:10px;color:var(--mt);margin-top:7px;line-height:1.5}

/* ── Buttons ─────────────────────────────────────────────────────────────── */
.qa{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:10px}
.ab{
  display:flex;align-items:center;gap:10px;padding:12px;border-radius:var(--r2);
  background:color-mix(in srgb,var(--bg3) 60%,transparent);border:1px solid var(--bd);
  color:var(--tx);font:inherit;font-size:12.5px;font-weight:600;cursor:pointer;text-align:left;
  transition:border-color .16s,transform .16s,box-shadow .16s,background .16s;
}
.ab .ic{width:31px;height:31px;border-radius:9px;flex:none;display:grid;place-items:center;
  background:color-mix(in srgb,var(--acc) 14%,transparent);color:var(--acc)}
.ab:hover{border-color:color-mix(in srgb,var(--acc) 45%,transparent);transform:translateY(-2px);box-shadow:var(--glow)}
.ab:active{transform:translateY(0)}
.ab:disabled{opacity:.45;cursor:not-allowed;transform:none;box-shadow:none}
.ab .sub{display:block;font-size:10px;font-weight:400;color:var(--mt);margin-top:1px}
.ab.danger .ic{background:color-mix(in srgb,var(--red) 14%,transparent);color:var(--red)}
.ab.danger:hover{border-color:color-mix(in srgb,var(--red) 50%,transparent);
  box-shadow:0 0 22px color-mix(in srgb,var(--red) 16%,transparent)}
.coffee{grid-column:1/-1;display:flex;align-items:center;justify-content:center;gap:8px;
  padding:10px;border-radius:var(--r2);border:1px dashed var(--bd2);color:var(--tx2);
  text-decoration:none;font-size:12px;transition:.16s}
.coffee:hover{color:var(--gold);border-color:color-mix(in srgb,var(--gold) 45%,transparent)}

.btn{display:inline-flex;align-items:center;justify-content:center;gap:7px;
  height:38px;padding:0 15px;border-radius:10px;font:inherit;font-size:12.5px;font-weight:600;
  cursor:pointer;border:1px solid var(--bd);background:var(--sf);color:var(--tx);transition:.16s}
.btn:hover{border-color:var(--bd2)}
.btn:active{transform:translateY(1px)}
.btn:disabled{opacity:.5;cursor:not-allowed}
.btn-p{background:linear-gradient(135deg,var(--acc),var(--blu));color:#04070d;border:none;font-weight:700}
.btn-p:hover{box-shadow:var(--glow)}
.btn-d{color:var(--red);border-color:color-mix(in srgb,var(--red) 40%,transparent)}
.btn-d:hover{background:color-mix(in srgb,var(--red) 12%,transparent)}

/* ── Views ───────────────────────────────────────────────────────────────── */
.view{display:none}
.view.on{display:block;animation:fade .22s ease}
@keyframes fade{from{opacity:0;transform:translateY(5px)}to{opacity:1;transform:none}}

/* ── Forms / modal / toast ───────────────────────────────────────────────── */
.mo{position:fixed;inset:0;z-index:100;display:none;align-items:center;justify-content:center;padding:18px;
  background:rgba(2,4,10,.72);backdrop-filter:blur(5px)}
.mo.show{display:flex}
.md{width:100%;max-width:600px;max-height:90vh;display:flex;flex-direction:column;
  background:var(--sf2);backdrop-filter:blur(20px);border:1px solid var(--bd2);border-radius:var(--r);
  box-shadow:0 30px 80px rgba(0,0,0,.6);animation:pop .2s ease}
@keyframes pop{from{opacity:0;transform:scale(.96) translateY(10px)}to{opacity:1;transform:none}}
.md-h{display:flex;align-items:center;gap:10px;padding:16px 18px;border-bottom:1px solid var(--bd);flex:none}
.md-t{display:flex;align-items:center;gap:9px;font-weight:650;font-size:15px;flex:1}
.md-x{background:none;border:none;color:var(--mt);cursor:pointer;font-size:19px;line-height:1;padding:4px 7px;border-radius:7px}
.md-x:hover{color:var(--tx);background:var(--bd)}
.md-b{padding:18px;overflow-y:auto;flex:1}
.md-f{display:flex;justify-content:flex-end;gap:9px;padding:14px 18px;border-top:1px solid var(--bd);flex:none}
.fg{margin-bottom:13px}
.fl{display:block;font-size:11px;font-weight:600;color:var(--tx2);margin-bottom:5px}
.fi{width:100%;height:38px;padding:0 11px;border-radius:9px;background:var(--bg3);
  border:1px solid var(--bd);color:var(--tx);font:inherit;font-size:13px;transition:.15s}
.fi:focus{outline:none;border-color:color-mix(in srgb,var(--acc) 55%,transparent);box-shadow:var(--glow)}
select.fi{cursor:pointer}
input[type=file].fi{padding:7px 10px;height:auto}
.fr2{display:grid;grid-template-columns:1fr 1fr;gap:11px}
@media(max-width:520px){.fr2{grid-template-columns:1fr}}
.fc{display:flex;align-items:center;gap:8px;margin-bottom:11px;font-size:12.5px}
.fc input{width:16px;height:16px;accent-color:var(--acc);cursor:pointer}
.hint{font-size:11px;color:var(--mt);line-height:1.5}
.sec{border-top:1px solid var(--bd);margin-top:15px;padding-top:13px}
.sec-t{display:flex;align-items:center;gap:8px;font-weight:650;margin-bottom:9px;font-size:13px}
code{font-family:ui-monospace,monospace;font-size:11px;background:var(--bd);padding:1px 4px;border-radius:4px}
.op-w{display:none;margin-top:14px}
.pt{height:7px;border-radius:5px;background:var(--bd);overflow:hidden}
.pf{height:100%;width:0;border-radius:5px;background:linear-gradient(90deg,var(--acc),var(--pur));transition:width .25s}
.pm{font-size:11.5px;color:var(--tx2);margin-top:7px;text-align:center}

.toast{position:fixed;bottom:22px;left:50%;transform:translate(-50%,26px);z-index:200;
  display:flex;align-items:center;gap:9px;padding:11px 16px;border-radius:11px;
  background:var(--sf2);backdrop-filter:blur(16px);border:1px solid var(--bd2);
  font-size:12.5px;font-weight:600;box-shadow:0 16px 40px rgba(0,0,0,.5);
  opacity:0;pointer-events:none;transition:.24s cubic-bezier(.4,0,.2,1)}
.toast.show{opacity:1;transform:translate(-50%,0)}
.t-ok{border-color:color-mix(in srgb,var(--grn) 45%,transparent);color:var(--grn)}
.t-err{border-color:color-mix(in srgb,var(--red) 45%,transparent);color:var(--red)}
.t-warn{border-color:color-mix(in srgb,var(--gold) 45%,transparent);color:var(--gold)}

.ft{display:flex;flex-wrap:wrap;gap:12px;justify-content:space-between;
  padding:14px 22px;border-top:1px solid var(--bd);color:var(--mt);font-size:11px;flex:none}

.fbar{display:flex;gap:8px;margin-bottom:14px;flex-wrap:wrap}
.fbar .fi{flex:1;min-width:190px;height:38px}

/* ── Mobile ──────────────────────────────────────────────────────────────── */
.scrim{position:fixed;inset:0;z-index:39;background:rgba(2,4,10,.6);backdrop-filter:blur(3px);
  opacity:0;pointer-events:none;transition:.2s}
@media(max-width:900px){
  .sb{transform:translateX(-100%);width:var(--sbw);transition:transform .24s cubic-bezier(.4,0,.2,1)}
  body.nav-open .sb{transform:none}
  body.nav-open .scrim{opacity:1;pointer-events:auto}
  .main,body.rail .main{margin-left:0}
  body.rail .sb{width:var(--sbw)}
  body.rail .sb-logo .txt,body.rail .nav-item .lbl,body.rail .hp-txt,body.rail .hp-sub{opacity:1;width:auto;display:block}
  body.rail .nav-item{justify-content:flex-start;padding:9px 11px}
  body.rail .nav-grp{opacity:1;height:auto;padding:16px 10px 7px}
  .ct{padding:16px}
  .ph1 h1{font-size:22px}
  .chart-wrap{height:210px}
}
/* Below 1100px the pool switcher crowds the header; the Pools view has the same action. */
@media(max-width:1100px){#poolSel{display:none}}
@media(min-width:901px){.mob-only{display:none!important}}
@media(max-width:900px){.desk-only{display:none!important}}
@media(max-width:600px){
  .kpis{grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:10px}
  .badge.b-ver{display:none}
}
</style>
<svg width="0" height="0" style="position:absolute" aria-hidden="true"><defs>
<symbol id="i-bolt" viewBox="0 0 24 24"><path d="M13 2 4.5 13.5H11l-1 8.5 8.5-11.5H12l1-8.5z"/></symbol>
<symbol id="i-grid" viewBox="0 0 24 24"><rect x="3" y="3" width="7" height="7" rx="1.5"/><rect x="14" y="3" width="7" height="7" rx="1.5"/><rect x="3" y="14" width="7" height="7" rx="1.5"/><rect x="14" y="14" width="7" height="7" rx="1.5"/></symbol>
<symbol id="i-net" viewBox="0 0 24 24"><rect x="9" y="3" width="6" height="5" rx="1.5"/><rect x="2" y="16" width="6" height="5" rx="1.5"/><rect x="16" y="16" width="6" height="5" rx="1.5"/><path d="M12 8v4M5 16v-2h14v2"/></symbol>
<symbol id="i-cpu" viewBox="0 0 24 24"><rect x="6" y="6" width="12" height="12" rx="2"/><rect x="9.5" y="9.5" width="5" height="5" rx="1"/><path d="M9 3v3M15 3v3M9 18v3M15 18v3M3 9h3M3 15h3M18 9h3M18 15h3"/></symbol>
<symbol id="i-server" viewBox="0 0 24 24"><rect x="3" y="4" width="18" height="6" rx="2"/><rect x="3" y="14" width="18" height="6" rx="2"/><path d="M7 7h.01M7 17h.01"/></symbol>
<symbol id="i-chart" viewBox="0 0 24 24"><path d="M3 3v16.5A1.5 1.5 0 0 0 4.5 21H21"/><path d="M7 15l3.5-4 3 2.5L20 7"/></symbol>
<symbol id="i-coins" viewBox="0 0 24 24"><ellipse cx="9" cy="6" rx="6" ry="3"/><path d="M3 6v5c0 1.7 2.7 3 6 3s6-1.3 6-3V6"/><path d="M3 11v5c0 1.7 2.7 3 6 3 1.2 0 2.3-.2 3.2-.5"/><circle cx="17" cy="16" r="4"/></symbol>
<symbol id="i-bell" viewBox="0 0 24 24"><path d="M6 9a6 6 0 1 1 12 0c0 4 1.5 5.5 2 6H4c.5-.5 2-2 2-6z"/><path d="M10 19a2 2 0 0 0 4 0"/></symbol>
<symbol id="i-term" viewBox="0 0 24 24"><rect x="3" y="4" width="18" height="16" rx="2"/><path d="M7 9l3 3-3 3M13 15h4"/></symbol>
<symbol id="i-cog" viewBox="0 0 24 24"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.6 1.6 0 0 0 .3 1.8l.1.1a2 2 0 1 1-2.8 2.8l-.1-.1a1.6 1.6 0 0 0-2.7 1.1v.3a2 2 0 1 1-4 0v-.2a1.6 1.6 0 0 0-2.8-1.1l-.1.1a2 2 0 1 1-2.8-2.8l.1-.1A1.6 1.6 0 0 0 3.5 15H3a2 2 0 1 1 0-4h.2a1.6 1.6 0 0 0 1.1-2.8l-.1-.1a2 2 0 1 1 2.8-2.8l.1.1a1.6 1.6 0 0 0 2.7-1.1V3a2 2 0 1 1 4 0v.2a1.6 1.6 0 0 0 2.8 1.1l.1-.1a2 2 0 1 1 2.8 2.8l-.1.1a1.6 1.6 0 0 0 1.1 2.7h.3a2 2 0 1 1 0 4h-.2a1.6 1.6 0 0 0-1.2 1z"/></symbol>
<symbol id="i-act" viewBox="0 0 24 24"><path d="M3 12h4l3 8 4-16 3 8h4"/></symbol>
<symbol id="i-check" viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M8.5 12.5l2.5 2.5 4.5-5"/></symbol>
<symbol id="i-x" viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M9 9l6 6M15 9l-6 6"/></symbol>
<symbol id="i-clock" viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M12 7v5l3 2"/></symbol>
<symbol id="i-db" viewBox="0 0 24 24"><ellipse cx="12" cy="5.5" rx="8" ry="3"/><path d="M4 5.5v13c0 1.7 3.6 3 8 3s8-1.3 8-3v-13"/><path d="M4 12c0 1.7 3.6 3 8 3s8-1.3 8-3"/></symbol>
<symbol id="i-shield" viewBox="0 0 24 24"><path d="M12 3l7 3v5c0 4.5-3 8.5-7 10-4-1.5-7-5.5-7-10V6l7-3z"/><path d="M12 11v3"/><circle cx="12" cy="9" r="1"/></symbol>
<symbol id="i-search" viewBox="0 0 24 24"><circle cx="11" cy="11" r="7"/><path d="M20 20l-3.5-3.5"/></symbol>
<symbol id="i-paint" viewBox="0 0 24 24"><path d="M12 3a9 9 0 1 0 0 18c1 0 1.5-.7 1.5-1.5 0-.4-.2-.8-.4-1-.3-.3-.4-.6-.4-1 0-.8.7-1.5 1.5-1.5H16a5 5 0 0 0 5-5c0-4.4-4-8-9-8z"/><circle cx="7.5" cy="11" r="1"/><circle cx="12" cy="7.5" r="1"/><circle cx="16.5" cy="11" r="1"/></symbol>
<symbol id="i-refresh" viewBox="0 0 24 24"><path d="M21 12a9 9 0 1 1-2.6-6.4"/><path d="M21 4v5h-5"/></symbol>
<symbol id="i-upload" viewBox="0 0 24 24"><path d="M12 16V4M8 8l4-4 4 4"/><path d="M4 16v2a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2v-2"/></symbol>
<symbol id="i-plug" viewBox="0 0 24 24"><path d="M9 3v6M15 3v6"/><path d="M6 9h12v2a6 6 0 0 1-12 0V9z"/><path d="M12 17v4"/></symbol>
<symbol id="i-trash" viewBox="0 0 24 24"><path d="M4 6h16M9 6V4h6v2M6 6l1 14h10l1-14"/><path d="M10 10v6M14 10v6"/></symbol>
<symbol id="i-coffee" viewBox="0 0 24 24"><path d="M4 8h13v6a5 5 0 0 1-10 0V8z" transform="translate(-1 0)"/><path d="M16 9h2a2.5 2.5 0 0 1 0 5h-2"/><path d="M4 21h14"/></symbol>
<symbol id="i-therm" viewBox="0 0 24 24"><path d="M10 13V5a2 2 0 1 1 4 0v8a4.5 4.5 0 1 1-4 0z"/><circle cx="12" cy="17" r="1.5" fill="currentColor" stroke="none"/></symbol>
<symbol id="i-wifi" viewBox="0 0 24 24"><path d="M2 8.5a16 16 0 0 1 20 0M5 12.5a11 11 0 0 1 14 0M8.5 16.5a6 6 0 0 1 7 0"/><circle cx="12" cy="20" r="1" fill="currentColor" stroke="none"/></symbol>
<symbol id="i-spark" viewBox="0 0 24 24"><path d="M12 2.5l2 5.5 5.5 2-5.5 2-2 5.5-2-5.5L4.5 10 10 8z"/><path d="M18.5 16l.8 2.2 2.2.8-2.2.8-.8 2.2-.8-2.2-2.2-.8 2.2-.8z"/></symbol>
<symbol id="i-up" viewBox="0 0 24 24"><path d="M12 19V5M6 11l6-6 6 6"/></symbol>
<symbol id="i-dn" viewBox="0 0 24 24"><path d="M12 5v14M6 13l6 6 6-6"/></symbol>
<symbol id="i-menu" viewBox="0 0 24 24"><path d="M4 7h16M4 12h16M4 17h16"/></symbol>
<symbol id="i-panel" viewBox="0 0 24 24"><rect x="3" y="4" width="18" height="16" rx="2"/><path d="M9 4v16"/></symbol>
<symbol id="i-chev" viewBox="0 0 24 24"><path d="M6 9l6 6 6-6"/></symbol>
<symbol id="i-warn" viewBox="0 0 24 24"><path d="M10.3 3.9 2.6 17a2 2 0 0 0 1.7 3h15.4a2 2 0 0 0 1.7-3L13.7 3.9a2 2 0 0 0-3.4 0z"/><path d="M12 9v4M12 17h.01"/></symbol>
<symbol id="i-block" viewBox="0 0 24 24"><path d="M12 2.8 20 7v10l-8 4.2L4 17V7z"/><path d="M4 7l8 4.2L20 7M12 11.2V21"/></symbol>
<symbol id="i-link" viewBox="0 0 24 24"><path d="M10 13a4 4 0 0 0 5.7 0l3-3A4 4 0 0 0 13 4.3l-1.7 1.7"/><path d="M14 11a4 4 0 0 0-5.7 0l-3 3A4 4 0 0 0 11 19.7l1.7-1.7"/></symbol>
<symbol id="i-heart" viewBox="0 0 24 24"><path d="M12 20s-7-4.4-7-9.5A4 4 0 0 1 12 7a4 4 0 0 1 7 3.5C19 15.6 12 20 12 20z"/></symbol>
<symbol id="i-gauge" viewBox="0 0 24 24"><path d="M4 17a8 8 0 1 1 16 0"/><path d="M12 17l4-5"/><circle cx="12" cy="17" r="1.5" fill="currentColor" stroke="none"/></symbol>
<symbol id="i-mem" viewBox="0 0 24 24"><rect x="4" y="7" width="16" height="10" rx="2"/><path d="M8 7V4M12 7V4M16 7V4M8 20v-3M12 20v-3M16 20v-3"/></symbol>
<symbol id="i-inbox" viewBox="0 0 24 24"><path d="M3 12h5l1.5 3h5L16 12h5"/><path d="M5.5 5h13l2.5 7v5a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-5z"/></symbol>
</defs></svg>

<aside class="sb" id="sb">
  <div class="sb-logo">
    <span class="mark"><svg class="i" viewBox="0 0 24 24" style="fill:currentColor;stroke:none"><use href="#i-bolt"/></svg></span>
    <span class="txt">NerdMiner</span>
  </div>
  <nav class="sb-nav" id="sbNav" aria-label="Main">
    <div class="nav-grp">Monitor</div>
    <div class="nav-item active" data-v="overview" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-grid"/></svg><span class="lbl">Overview</span></div>
    <div class="nav-item" data-v="fleet" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-net"/></svg><span class="lbl">Fleet</span></div>
    <div class="nav-item" data-v="workers" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-cpu"/></svg><span class="lbl">Workers</span></div>
    <div class="nav-item" data-v="pools" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-server"/></svg><span class="lbl">Pools</span></div>
    <div class="nav-grp">Analytics</div>
    <div class="nav-item" data-v="stats" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-chart"/></svg><span class="lbl">Statistics</span></div>
    <div class="nav-item" data-v="shares" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-coins"/></svg><span class="lbl">Shares</span></div>
    <div class="nav-grp">System</div>
    <div class="nav-item" data-v="alerts" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-bell"/></svg><span class="lbl">Alerts</span><span class="pip" id="navAlerts" style="display:none">0</span></div>
    <div class="nav-item" data-v="logs" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-term"/></svg><span class="lbl">Logs</span></div>
    <div class="nav-item" data-act="cfg" role="button" tabindex="0"><svg class="i" viewBox="0 0 24 24"><use href="#i-cog"/></svg><span class="lbl">Settings</span></div>
  </nav>
  <div class="sb-foot">
    <div class="hp">
      <span class="hp-dot" id="healthDot"></span>
      <div><div class="hp-txt" id="healthTxt">Connecting</div></div>
    </div>
    <div class="hp-sub" id="healthSub">Waiting for first poll</div>
  </div>
</aside>
<div class="scrim" id="scrim"></div>

<div class="main">
  <header class="tb">
    <button class="icon-btn mob-only" id="btnMenu" aria-label="Open navigation"><svg class="i big" viewBox="0 0 24 24"><use href="#i-menu"/></svg></button>
    <button class="icon-btn desk-only" id="btnRail" aria-label="Collapse sidebar" title="Collapse sidebar (\)"><svg class="i big" viewBox="0 0 24 24"><use href="#i-panel"/></svg></button>
    <span class="tb-host" id="tbHost">NerdMiner</span>
    <div class="sp"></div>
    <div class="tb-r">
      <span class="badge b-ok" id="connBadge"><span class="bdot"></span><span id="connTxt">Connecting</span></span>
      <span class="badge b-vpn" id="vpnBadge" title="WireGuard VPN tunnel" style="display:none"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-shield"/></svg>VPN</span>
      <span class="badge b-ver" id="verBadge">v—</span>
      <select class="fi" id="poolSel" style="height:30px;width:auto;max-width:170px;font-size:11.5px" title="Switch this miner to another pool">
        <option value="">— Switch pool —</option>
      </select>
      <div class="dd" id="ddTheme">
        <button class="icon-btn" id="btnTheme" aria-label="Customize theme" title="Customize"><svg class="i big" viewBox="0 0 24 24"><use href="#i-paint"/></svg></button>
        <div class="dd-menu" role="menu">
          <div class="dd-h">Theme</div>
          <button class="dd-i" data-theme="aurora"><span class="sw" style="background:linear-gradient(135deg,#22d3ee,#8b5cf6)"></span>Aurora<svg class="i sm ck" viewBox="0 0 24 24"><use href="#i-check"/></svg></button>
          <button class="dd-i" data-theme="classic"><span class="sw" style="background:linear-gradient(135deg,#f4d03f,#60a5fa)"></span>Classic<svg class="i sm ck" viewBox="0 0 24 24"><use href="#i-check"/></svg></button>
          <button class="dd-i" data-theme="matrix"><span class="sw" style="background:linear-gradient(135deg,#00ff85,#0a3d20)"></span>Cyber Matrix<svg class="i sm ck" viewBox="0 0 24 24"><use href="#i-check"/></svg></button>
          <button class="dd-i" data-theme="synthwave"><span class="sw" style="background:linear-gradient(135deg,#ff71ce,#01cdfe)"></span>Synthwave<svg class="i sm ck" viewBox="0 0 24 24"><use href="#i-check"/></svg></button>
          <button class="dd-i" data-theme="nord"><span class="sw" style="background:linear-gradient(135deg,#88c0d0,#5e81ac)"></span>Nord<svg class="i sm ck" viewBox="0 0 24 24"><use href="#i-check"/></svg></button>
          <div class="dd-sep"></div>
          <button class="dd-i" data-act="cfg"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-cog"/></svg>Settings</button>
          <button class="dd-i" data-act="ota"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-upload"/></svg>Firmware update</button>
          <button class="dd-i" data-act="restart"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-refresh"/></svg>Restart miner</button>
        </div>
      </div>
    </div>
  </header>
  <main class="ct">
    <div class="ph1">
      <h1 id="pgT">Overview</h1>
      <span class="sub" id="pgS">Real-time overview of your mining operation</span>
      <span class="sp"></span>
      <div class="ranges" id="ranges" role="tablist" aria-label="Chart range">
        <button class="rg" data-r="1h" role="tab">1H</button>
        <button class="rg" data-r="6h" role="tab">6H</button>
        <button class="rg active" data-r="24h" role="tab" aria-selected="true">24H</button>
        <button class="rg" data-r="7d" role="tab">7D</button>
        <button class="rg" data-r="30d" role="tab">30D</button>
      </div>
    </div>

    <!-- ── Overview ──────────────────────────────────────────────────────── -->
    <section class="view on" id="v-overview">
      <div class="kpis">
        <article class="card kpi" style="--k:var(--acc)">
          <div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-act"/></svg>Hashrate</div>
          <div class="kv sk" id="kHash">000.0</div>
          <div class="kf"><span class="delta fl" id="kHashD">—</span><span>vs 5 min ago</span></div>
        </article>
        <article class="card kpi" style="--k:var(--grn)">
          <div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-check"/></svg>Accepted</div>
          <div class="kv sk" id="kAcc">0</div>
          <div class="kf"><span id="kAccR">— accept rate</span></div>
        </article>
        <article class="card kpi" style="--k:var(--red)">
          <div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-x"/></svg>Rejected</div>
          <div class="kv" id="kRej">0</div>
          <div class="kf"><span id="kRejR">this session</span></div>
        </article>
        <article class="card kpi" style="--k:var(--blu)">
          <div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-clock"/></svg>Uptime</div>
          <div class="kv sm sk" id="kUp">—</div>
          <div class="kf"><span id="kUpS2">since boot</span></div>
        </article>
        <article class="card kpi" style="--k:var(--gold)" id="kTempCard">
          <div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-therm"/></svg>Temperature</div>
          <div class="kv sk" id="kTemp">—</div>
          <div class="kf"><span class="delta fl" id="kTempD">—</span><span>on-die sensor</span></div>
        </article>
        <article class="card kpi" style="--k:var(--pur)">
          <div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-mem"/></svg>Free heap</div>
          <div class="kv sm sk" id="kHeap">—</div>
          <div class="kf"><span id="kHeapS">of total</span></div>
        </article>
      </div>

      <div class="gridC" style="margin-bottom:16px">
        <article class="card chart-card">
          <div class="ph">
            <div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-chart"/></svg></span>Hashrate Performance</div>
            <span class="sp"></span>
            <span class="live-tag"><span class="ldot"></span>LIVE</span>
            <span class="badge b-ver" id="chartSrc" title="Where this series comes from">client</span>
          </div>
          <div class="chart-wrap" id="chartWrap">
            <svg id="hrSvg" preserveAspectRatio="none" role="img" aria-label="Hashrate over time"></svg>
            <div class="tip" id="tip"><div class="tv" id="tipV">—</div><div class="tt" id="tipT">—</div></div>
          </div>
          <div class="cstrip">
            <div class="ci"><div class="ci-l">Current</div><div class="ci-v" id="csN">—</div></div>
            <div class="ci"><div class="ci-l">Average</div><div class="ci-v" id="csA">—</div></div>
            <div class="ci"><div class="ci-l" style="color:var(--grn)">Max</div><div class="ci-v" style="color:var(--grn)" id="csHi">—</div></div>
            <div class="ci"><div class="ci-l" style="color:var(--red)">Min</div><div class="ci-v" style="color:var(--red)" id="csLo">—</div></div>
            <div class="ci"><div class="ci-l" style="color:var(--gold)">Variance</div><div class="ci-v" style="color:var(--gold)" id="csV">—</div></div>
          </div>
        </article>

        <article class="card">
          <div class="ph"><div class="ph-t"><span class="ic" style="background:color-mix(in srgb,var(--blu) 14%,transparent);color:var(--blu)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-server"/></svg></span>Pool Connection</div></div>
          <div class="pp-hdr">
            <span class="pp-icon"><svg class="i big" viewBox="0 0 24 24"><use href="#i-server"/></svg></span>
            <div style="min-width:0">
              <div class="pp-name" id="ppName">—</div>
              <div class="pp-url" id="ppUrl">—</div>
            </div>
          </div>
          <div class="pr"><span class="pr-k">Status</span><span class="pr-v vg" id="ppSt">—</span></div>
          <div class="pr"><span class="pr-k">Worker</span><span class="pr-v mono" id="ppWk">—</span></div>
          <div class="pr"><span class="pr-k">Wallet</span><span class="pr-v mono" id="ppWa">—</span></div>
          <div class="pr"><span class="pr-k">Best difficulty</span><span class="pr-v vb" id="ppDf">—</span></div>
          <div class="pr"><span class="pr-k">Last share</span><span class="pr-v" id="ppLs">—</span></div>
          <div class="pr"><span class="pr-k">Round-trip latency</span><span class="pr-v" id="ppLat">—</span></div>
          <div class="dbars" id="dbars"></div>
          <div class="dbars-lbl" id="dbarsLbl">Share difficulty — waiting for the first accepted share</div>
        </article>
      </div>

      <div class="gridC" style="margin-bottom:16px">
        <article class="card">
          <div class="ph"><div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-gauge"/></svg></span>System Health</div></div>
          <div style="display:flex;gap:18px;align-items:center;flex-wrap:wrap">
            <div class="gauge">
              <svg viewBox="0 0 42 42" aria-label="Overall health score">
                <circle cx="21" cy="21" r="17" fill="none" stroke="var(--bd)" stroke-width="3.4"/>
                <circle id="gArc" cx="21" cy="21" r="17" fill="none" stroke="var(--grn)" stroke-width="3.4"
                        stroke-linecap="round" stroke-dasharray="106.8" stroke-dashoffset="106.8"
                        transform="rotate(-90 21 21)" style="transition:stroke-dashoffset .7s cubic-bezier(.4,0,.2,1),stroke .3s"/>
              </svg>
              <div class="gc"><div><div class="gn" id="gNum">—</div><div class="gl">Health score</div></div></div>
            </div>
            <div class="htiles" style="flex:1;min-width:280px">
              <div class="ht"><span class="ic" style="background:color-mix(in srgb,var(--acc) 14%,transparent);color:var(--acc)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-cpu"/></svg></span>
                <div class="ht-v" id="hChip">—</div><div class="ht-l">Chip</div><div class="ht-s" style="color:var(--tx2)" id="hCpu">—</div></div>
              <div class="ht"><span class="ic" style="background:color-mix(in srgb,var(--pur) 14%,transparent);color:var(--pur)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-mem"/></svg></span>
                <div class="ht-v" id="hMemP">—</div><div class="ht-l">Memory used</div>
                <div class="mb-t"><div class="mb-f" id="hMemB" style="width:0"></div></div></div>
              <div class="ht"><span class="ic" style="background:color-mix(in srgb,var(--gold) 14%,transparent);color:var(--gold)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-db"/></svg></span>
                <div class="ht-v" id="hFlash">—</div><div class="ht-l">Flash size</div><div class="ht-s" style="color:var(--tx2)" id="hOta">—</div></div>
              <div class="ht"><span class="ic" style="background:color-mix(in srgb,var(--grn) 14%,transparent);color:var(--grn)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-wifi"/></svg></span>
                <div class="ht-v" id="hRssi">—</div><div class="ht-l">Signal</div><div class="ht-s" id="hRssiQ">—</div></div>
              <div class="ht"><span class="ic" style="background:color-mix(in srgb,var(--blu) 14%,transparent);color:var(--blu)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-link"/></svg></span>
                <div class="ht-v mono" id="hIp">—</div><div class="ht-l">IP address</div><div class="ht-s" style="color:var(--tx2)" id="hHost">—</div></div>
              <div class="ht"><span class="ic" style="background:color-mix(in srgb,var(--acc) 14%,transparent);color:var(--acc)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-gauge"/></svg></span>
                <div class="ht-v" id="hLat">—</div><div class="ht-l">API latency</div><div class="ht-s" style="color:var(--tx2)">to this device</div></div>
              <div class="ht"><span class="ic" style="background:color-mix(in srgb,var(--pur) 14%,transparent);color:var(--pur)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-cpu"/></svg></span>
                <div class="ht-v" id="hTpl">—</div><div class="ht-l">Templates</div><div class="ht-s" style="color:var(--tx2)">jobs received</div></div>
              <div class="ht"><span class="ic" style="background:color-mix(in srgb,var(--grn) 14%,transparent);color:var(--grn)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-check"/></svg></span>
                <div class="ht-v" id="hEff">—</div><div class="ht-l">Efficiency</div><div class="ht-s" style="color:var(--tx2)">accepted / total</div></div>
            </div>
          </div>
        </article>

        <article class="card">
          <div class="ph">
            <div class="ph-t"><span class="ic" style="background:color-mix(in srgb,var(--acc) 14%,transparent);color:var(--acc)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-act"/></svg></span>Activity Feed</div>
            <span class="sp"></span>
            <span class="live-tag"><span class="ldot"></span>LIVE</span>
          </div>
          <div class="feed" id="feed"></div>
        </article>
      </div>

      <article class="card">
        <div class="ph"><div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-cog"/></svg></span>Quick Actions</div></div>
        <div class="qa">
          <button class="ab" data-act="cfg"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-cog"/></svg></span><span>Configure<span class="sub">Pool, wallet, VPN</span></span></button>
          <button class="ab" data-act="ota"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-upload"/></svg></span><span>Firmware OTA<span class="sub">Upload a .bin</span></span></button>
          <button class="ab" data-act="test"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-plug"/></svg></span><span>Test pool<span class="sub">Check reachability</span></span></button>
          <button class="ab" data-act="backup"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-db"/></svg></span><span>Backup config<span class="sub">Download JSON</span></span></button>
          <button class="ab" data-act="restart"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-refresh"/></svg></span><span>Restart miner<span class="sub">Reboot now</span></span></button>
          <button class="ab danger" data-act="reset"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-trash"/></svg></span><span>Factory reset<span class="sub">Erases all settings</span></span></button>
          <a class="coffee" href="https://buymeacoffee.com/sx8yfh9zrbs" target="_blank" rel="noopener"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-coffee"/></svg>Buy me a coffee</a>
        </div>
      </article>
    </section>
    <!-- ── Fleet ─────────────────────────────────────────────────────────── -->
    <section class="view" id="v-fleet">
      <div class="kpis" style="grid-template-columns:repeat(auto-fit,minmax(200px,1fr))">
        <article class="card kpi" style="--k:var(--grn)"><div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-cpu"/></svg>Online</div><div class="kv" id="flOnline">—</div><div class="kf"><span>miners reachable</span></div></article>
        <article class="card kpi" style="--k:var(--acc)"><div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-act"/></svg>Total hashrate</div><div class="kv" id="flHash">—</div><div class="kf"><span>combined</span></div></article>
        <article class="card kpi" style="--k:var(--blu)"><div class="kh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-check"/></svg>Total accepted</div><div class="kv" id="flShares">—</div><div class="kf"><span>shares</span></div></article>
      </div>
      <article class="card" style="margin-bottom:16px">
        <div class="ph"><div class="ph-t"><span class="ic" style="background:color-mix(in srgb,var(--gold) 14%,transparent);color:var(--gold)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-coins"/></svg></span>Pool Distribution</div></div>
        <div class="donut-wrap">
          <div class="donut">
            <svg viewBox="0 0 42 42" id="donut" aria-label="Pool distribution across fleet"></svg>
            <div class="dc"><div><div class="dn" id="dnN">—</div><div class="dl">Pools</div></div></div>
          </div>
          <div class="lg" id="poolLegend"></div>
        </div>
        <div class="dbars-lbl" style="margin-top:12px">Share of combined fleet hashrate by pool.</div>
      </article>
      <article class="card">
        <div class="ph">
          <div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-net"/></svg></span>Fleet — all miners</div>
          <span class="sp"></span><span class="badge b-ver" id="fleetCount">0 miners</span>
        </div>
        <div class="fbar">
          <input class="fi" id="fleetInput" type="text" placeholder="Add miner by IP or hostname (e.g. 192.168.1.50)">
          <button class="btn btn-p" data-act="fleetAdd">Add</button>
          <button class="btn" id="fleetScanBtn" data-act="fleetScan">Scan LAN</button>
          <button class="btn" data-act="fleetRefresh"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-refresh"/></svg>Refresh</button>
          <button class="btn btn-d" id="fleetRestartBtn" data-act="fleetRestart">Restart all</button>
        </div>
        <div class="tw"><table class="st">
          <thead><tr><th>Miner</th><th>IP</th><th>Status</th><th>Hashrate</th><th>Accepted</th><th>Best diff</th><th>Uptime</th><th>Version</th><th></th></tr></thead>
          <tbody id="fleetBody"></tbody>
        </table></div>
        <div class="dbars-lbl" style="margin-top:12px"><strong>Scan LAN</strong> auto-discovers miners on this device's subnet via mDNS, falling back to a /24 sweep. Stats are polled directly from each miner's <code>/api/status</code>, so all miners must be on this network. To update firmware, open a miner's own dashboard — one <code>.bin</code> only fits one board.</div>
      </article>
    </section>

    <!-- ── Workers ───────────────────────────────────────────────────────── -->
    <section class="view" id="v-workers">
      <article class="card">
        <div class="ph"><div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-cpu"/></svg></span>This device</div></div>
        <div class="htiles" id="wkTiles"></div>
      </article>
    </section>

    <!-- ── Pools ─────────────────────────────────────────────────────────── -->
    <section class="view" id="v-pools">
      <article class="card">
        <div class="ph"><div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-server"/></svg></span>Known pools</div><span class="sp"></span><span class="badge b-ver" id="poolCount">—</span></div>
        <div class="tw"><table class="st">
          <thead><tr><th>Pool</th><th>Host</th><th>Port</th><th>Stats API</th><th>Active</th></tr></thead>
          <tbody id="poolBody"></tbody>
        </table></div>
        <div class="dbars-lbl" style="margin-top:12px">Switching pool saves the new endpoint and restarts the miner.</div>
      </article>
    </section>

    <!-- ── Statistics ────────────────────────────────────────────────────── -->
    <section class="view" id="v-stats">
      <article class="card live">
        <div class="ph"><div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-chart"/></svg></span>Hashrate analytics</div><span class="sp"></span><span class="live-tag"><span class="ldot"></span>LIVE</span></div>
        <div class="chart-wrap" style="height:340px" id="chartWrap2">
          <svg id="hrSvg2" preserveAspectRatio="none" role="img" aria-label="Hashrate over time"></svg>
          <div class="tip" id="tip2"><div class="tv" id="tipV2">—</div><div class="tt" id="tipT2">—</div></div>
        </div>
        <div class="cstrip">
          <div class="ci"><div class="ci-l">Current</div><div class="ci-v" id="s2N">—</div></div>
          <div class="ci"><div class="ci-l">Average</div><div class="ci-v" id="s2A">—</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--grn)">Max</div><div class="ci-v" style="color:var(--grn)" id="s2Hi">—</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--red)">Min</div><div class="ci-v" style="color:var(--red)" id="s2Lo">—</div></div>
          <div class="ci"><div class="ci-l" style="color:var(--gold)">Variance</div><div class="ci-v" style="color:var(--gold)" id="s2V">—</div></div>
        </div>
      </article>
    </section>

    <!-- ── Shares ────────────────────────────────────────────────────────── -->
    <section class="view" id="v-shares">
      <article class="card">
        <div class="ph"><div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-coins"/></svg></span>Recent shares</div><span class="sp"></span><span class="badge b-ver" id="shareCount">0</span></div>
        <div class="tw"><table class="st" style="min-width:0">
          <thead><tr><th>Time</th><th>Result</th><th>Difficulty</th></tr></thead>
          <tbody id="sTbody"></tbody>
        </table></div>
      </article>
    </section>

    <!-- ── Alerts ────────────────────────────────────────────────────────── -->
    <section class="view" id="v-alerts">
      <article class="card" style="margin-bottom:16px">
        <div class="ph">
          <div class="ph-t"><span class="ic" style="background:color-mix(in srgb,var(--pur) 14%,transparent);color:var(--pur)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-spark"/></svg></span>Insights</div>
          <span class="sp"></span>
          <span class="badge b-ver" title="Rule-based, computed in your browser from live telemetry — not a language model">HEURISTIC</span>
        </div>
        <div class="ins" id="insList"></div>
      </article>
      <article class="card">
        <div class="ph"><div class="ph-t"><span class="ic" style="background:color-mix(in srgb,var(--gold) 14%,transparent);color:var(--gold)"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-bell"/></svg></span>Alerts &amp; events</div><span class="sp"></span><span class="badge b-ver" id="alertCount">0 events</span></div>
        <div class="feed" id="alertList" style="max-height:none"></div>
      </article>
    </section>

    <!-- ── Logs ──────────────────────────────────────────────────────────── -->
    <section class="view" id="v-logs">
      <article class="card">
        <div class="ph"><div class="ph-t"><span class="ic"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-term"/></svg></span>System log</div><span class="sp"></span><span class="badge b-ver" id="logCount">0 entries</span></div>
        <div class="feed" id="logList" style="max-height:none"></div>
      </article>
    </section>
  </main>

  <footer class="ft">
    <span id="ftL">Last updated —</span>
    <span id="ftC">Auto-refresh 3s</span>
    <span id="ftR">Save stats to flash (NVS): —</span>
  </footer>
</div>

<div class="toast" id="toast"></div>

<!-- ── Config modal ──────────────────────────────────────────────────────── -->
<div class="mo" id="moCfg" role="dialog" aria-modal="true" aria-labelledby="cfgTitle">
  <div class="md">
    <div class="md-h">
      <div class="md-t" id="cfgTitle"><svg class="i" viewBox="0 0 24 24"><use href="#i-cog"/></svg>Configure miner</div>
      <button class="md-x" data-close="moCfg" aria-label="Close">&#x2715;</button>
    </div>
    <div class="md-b">
      <div class="fg"><label class="fl" for="cWallet">Wallet address</label><input class="fi" id="cWallet" type="text" placeholder="bc1q… or 1… or 3…"></div>
      <div class="fr2">
        <div class="fg"><label class="fl" for="cUrl">Pool URL</label><input class="fi" id="cUrl" type="text" placeholder="public-pool.io"></div>
        <div class="fg"><label class="fl" for="cPort">Pool port</label><input class="fi" id="cPort" type="number" min="1" max="65535" placeholder="21496"></div>
      </div>
      <div class="fg"><label class="fl" for="cPass">Pool password</label><input class="fi" id="cPass" type="text" placeholder="x"></div>
      <div class="fr2">
        <div class="fg"><label class="fl" for="cTz">Timezone (UTC offset)</label><input class="fi" id="cTz" type="number" min="-12" max="14" placeholder="0"></div>
        <div class="fg"><label class="fl" for="cThemeSel">Theme — saved in this browser</label>
          <select class="fi" id="cThemeSel">
            <option value="aurora">Aurora (default)</option>
            <option value="classic">Classic (dark + gold)</option>
            <option value="matrix">Cyber Matrix</option>
            <option value="synthwave">Synthwave</option>
            <option value="nord">Nord</option>
          </select>
        </div>
      </div>
      <div class="fc"><input type="checkbox" id="cSave"><label for="cSave">Save mining stats to flash (NVS)</label></div>
      <div class="fg"><label class="fl" for="cToken">API token — only for firmware built with <code>WEBUI_AUTH_TOKEN</code>; saved in this browser, never in the miner's config</label><input class="fi" id="cToken" type="password" placeholder="leave empty if unused"></div>

      <div id="wgSection" class="sec" style="display:none">
        <div class="fc"><input type="checkbox" id="cWgEn"><label for="cWgEn"><strong>WireGuard VPN</strong> — full-tunnel (pool traffic + remote dashboard access)</label></div>
        <div class="fr2">
          <div class="fg"><label class="fl" for="cWgIp">Tunnel IP (this device)</label><input class="fi" id="cWgIp" type="text" placeholder="10.6.0.2"></div>
          <div class="fg"><label class="fl" for="cWgPort">Endpoint port</label><input class="fi" id="cWgPort" type="number" min="1" max="65535" placeholder="51820"></div>
        </div>
        <div class="fg"><label class="fl" for="cWgEp">Server endpoint (host or IP)</label><input class="fi" id="cWgEp" type="text" placeholder="vpn.example.com"></div>
        <div class="fg"><label class="fl" for="cWgPub">Server public key</label><input class="fi" id="cWgPub" type="text" placeholder="base64 peer public key"></div>
        <div class="fg"><label class="fl" for="cWgPriv">This device's private key <span id="cWgPkState" style="color:var(--mt)"></span></label><input class="fi" id="cWgPriv" type="password" placeholder="base64 private key"></div>
        <div class="fg"><label class="fl" for="cWgPsk">Preshared key <span id="cWgPskState" style="color:var(--mt)"></span></label><input class="fi" id="cWgPsk" type="password" placeholder="base64 preshared key (optional)"></div>
        <div class="fc"><input type="checkbox" id="cWgPskClear"><label for="cWgPskClear">Remove the stored preshared key (server has no <code>PresharedKey</code>)</label></div>
        <div class="hint">Copy every field from the server's peer config. If it has a <code>PresharedKey</code> line you <strong>must</strong> paste it here or the handshake will silently fail. Do not trust the generated <code>Endpoint</code> port if handshakes time out — confirm it against a working peer. Keys are stored on the miner and never sent back to the browser; leave a key box blank to keep the current one. Saving restarts the miner.</div>
      </div>

      <div class="sec">
        <div class="sec-t"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-bell"/></svg>Alerts — webhook notifications</div>
        <div class="fr2">
          <div class="fg"><label class="fl" for="cAlSvc">Service</label>
            <select class="fi" id="cAlSvc"><option value="discord">Discord</option><option value="ntfy">ntfy</option><option value="json">Generic (Slack/JSON)</option></select>
          </div>
          <div class="fg" style="display:flex;align-items:flex-end"><button class="btn" type="button" style="width:100%" data-act="alertTest">Send test</button></div>
        </div>
        <div class="fg"><label class="fl" for="cAlUrl">Webhook URL — leave empty to disable</label><input class="fi" id="cAlUrl" type="text" placeholder="https://discord.com/api/webhooks/… or https://ntfy.sh/your-topic"></div>
        <div class="hint">The miner posts a message when it finds a block, its pool or VPN drops, or it comes online. Sent over plain HTTPS without certificate checks. Use <strong>Send test</strong> to verify before saving.</div>
      </div>
      <div id="cMsg" class="hint" style="margin-top:11px"></div>
    </div>
    <div class="md-f">
      <button class="btn" data-close="moCfg">Cancel</button>
      <button class="btn btn-p" data-act="save">Save &amp; restart</button>
    </div>
  </div>
</div>

<!-- ── OTA modal ─────────────────────────────────────────────────────────── -->
<div class="mo" id="moOta" role="dialog" aria-modal="true" aria-labelledby="otaTitle">
  <div class="md" style="max-width:520px">
    <div class="md-h">
      <div class="md-t" id="otaTitle"><svg class="i" viewBox="0 0 24 24"><use href="#i-upload"/></svg>Firmware update (OTA)</div>
      <button class="md-x" data-close="moOta" aria-label="Close">&#x2715;</button>
    </div>
    <div class="md-b">
      <p class="hint" style="margin-bottom:13px">Upload the bare app image <strong style="color:var(--tx)">firmware.bin</strong> built for <strong style="color:var(--tx)"><span id="otaB">this board</span></strong> — not <code>*_factory.bin</code>, and not another board's image. The device restarts automatically. Current firmware: <span id="otaV" style="color:var(--acc)">—</span></p>
      <div style="display:flex;gap:9px;align-items:center">
        <input class="fi" type="file" id="otaFile" accept=".bin" style="flex:1">
        <button class="btn btn-p" data-act="otaStart">Flash</button>
      </div>
      <div class="op-w" id="otaW">
        <div class="pt"><div class="pf" id="otaFill"></div></div>
        <div class="pm" id="otaMsg">Uploading…</div>
      </div>
    </div>
    <div class="md-f"><button class="btn" data-close="moOta">Close</button></div>
  </div>
</div>
<script>
(function(){
'use strict';
var POLL=3000, MAX_HR=180, SPARK_N=40;

// ── State ───────────────────────────────────────────────────────────────────
var hrBuf=[], sparks={hash:[],temp:[]};  // back the KPI trend deltas only
var rejected=0, shareLog=[], lastShare=null, tz=0, saveStats=false;
var diffHist=[0,0,0,0,0,0,0], lastTs=null, lastEvT=0;
var alertLog=[], sysLog=[], prevConn=null, latency=null;
var currentView='overview', activePool='', lastWallet='', range='24h';
var histCache={}, histSrc='client', hasDeviceHistory=null, hasTemp=false;
var fleet=[], fleetData={}, fleetBusy=false, selfIp=null, lastStatus=null;
var selfAdded=false, scanning=false, fleetLoaded=false, fleetPendingSelf=null;
var FLEET_MAX=32, otaBusy=false, lastSys=null;

var POOLS_FALLBACK=[
  {name:'public-pool.io',host:'public-pool.io',port:21496},
  {name:'nerdminers.org',host:'pool.nerdminers.org',port:3333},
  {name:'sethforprivacy.com',host:'pool.sethforprivacy.com',port:3333},
  {name:'solomining.de',host:'pool.solomining.de',port:3333},
  {name:'pyblock.xyz',host:'pool.pyblock.xyz',port:3333}
];
var VIEWS={
  overview:{t:'Overview',s:'Real-time overview of your mining operation'},
  fleet:{t:'Fleet',s:'All miners on your network'},
  workers:{t:'Workers',s:'Single-device miner stats'},
  pools:{t:'Pools',s:'Pool connection & status'},
  stats:{t:'Statistics',s:'Hashrate analytics'},
  shares:{t:'Shares',s:'Submission log'},
  alerts:{t:'Alerts',s:'Events & notifications'},
  logs:{t:'Logs',s:'System activity log'}
};
var RANGE_LBL={'1h':'1 hour','6h':'6 hours','24h':'24 hours','7d':'7 days','30d':'30 days'};

// ── Utils ───────────────────────────────────────────────────────────────────
function el(id){return document.getElementById(id);}
function set(id,v){var e=el(id);if(e){e.textContent=v;e.classList.remove('sk');}}
function esc(s){return String(s).replace(/[&<>"']/g,function(c){
  return {'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c];});}
function fmtH(k){
  if(k>=1000)return (k/1000).toFixed(2)+' MH/s';
  if(k>=1)return k.toFixed(1)+' KH/s';
  return Math.round(k*1000)+' H/s';
}
function fmtHn(k){return k>=1000?(k/1000).toFixed(2):k.toFixed(1);}
function fmtHu(k){return k>=1000?'MH/s':'KH/s';}
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
  if(n>=1)return n.toFixed(2);
  return n.toExponential(2);
}
function trunc(s,n){if(!s||s.length<=n)return s||'—';return s.slice(0,6)+'…'+s.slice(-6);}
function nowT(){return new Date().toLocaleTimeString();}
function ago(ts){
  var s=Math.max(0,Math.round(Date.now()/1000-ts));
  if(s<60)return s+'s ago';
  if(s<3600)return Math.floor(s/60)+'m ago';
  if(s<86400)return Math.floor(s/3600)+'h ago';
  return Math.floor(s/86400)+'d ago';
}
function cvar(n){return getComputedStyle(document.documentElement).getPropertyValue(n).trim();}
function push(arr,v,n){arr.push(v);if(arr.length>(n||SPARK_N))arr.shift();}

var tTmr;
function toast(msg,type){
  var t=el('toast');
  t.textContent=msg;
  t.className='toast t-'+(type||'ok')+' show';
  clearTimeout(tTmr);
  tTmr=setTimeout(function(){t.classList.remove('show');},3200);
}

// Animated count-up. Skipped for reduced-motion and for the first paint.
var RM=window.matchMedia&&matchMedia('(prefers-reduced-motion:reduce)').matches;
var animLast={};
function animNum(id,to,fmt){
  var e=el(id);if(!e)return;
  e.classList.remove('sk');
  var from=animLast[id];
  animLast[id]=to;
  if(RM||from===undefined||from===to||Math.abs(to-from)<0.01){e.textContent=fmt(to);return;}
  var t0=performance.now(),dur=460;
  function step(t){
    // Clamp BOTH ends. A rAF timestamp can precede t0 (virtual time, clock
    // adjustment), and a negative p run through the cubic ease-out produces a
    // wildly out-of-range value — e.g. a 52°C reading rendered as -34°C.
    var p=Math.max(0,Math.min(1,(t-t0)/dur));
    p=1-Math.pow(1-p,3);
    var v=from+(to-from)*p;
    e.textContent=fmt(isFinite(v)?v:to);
    if(p<1)requestAnimationFrame(step);
  }
  requestAnimationFrame(step);
}

// ── Auth + fetch ────────────────────────────────────────────────────────────
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
function on401(){
  if(tokenPrompted)return;
  tokenPrompted=true;
  var t=window.prompt('This miner requires an API token (WEBUI_AUTH_TOKEN build).\nEnter it once — it is stored only in this browser:');
  if(t&&t.trim()){setToken(t);location.reload();}
  else toast('API token required — set it in Settings','err');
}
function api(url,opts){
  opts=opts||{};opts.headers=authHdrs(opts.headers);
  return fetch(url,opts).then(function(r){if(r.status===401)on401();return r;});
}
function fetchT(url,ms,opts){
  var ctrl=('AbortController' in window)?new AbortController():null;
  var o=opts||{};
  o.headers=authHdrs(o.headers);
  if(ctrl)o.signal=ctrl.signal;
  var tmr=ctrl?setTimeout(function(){ctrl.abort();},ms):null;
  var done=function(){if(tmr)clearTimeout(tmr);};
  return fetch(url,o).then(function(r){done();if(r.status===401)on401();return r;},function(e){done();throw e;});
}

// ── Theme ───────────────────────────────────────────────────────────────────
var THEMES=['aurora','classic','matrix','synthwave','nord'];
function currentTheme(){return document.documentElement.getAttribute('data-theme')||'aurora';}
function setTheme(t){
  if(THEMES.indexOf(t)<0)t='aurora';
  if(t==='aurora')document.documentElement.removeAttribute('data-theme');
  else document.documentElement.setAttribute('data-theme',t);
  try{localStorage.setItem('nm_theme',t==='aurora'?'':t);}catch(e){}
  document.querySelectorAll('.dd-i[data-theme]').forEach(function(b){
    b.classList.toggle('on',b.dataset.theme===t);
  });
  var s=el('cThemeSel');if(s)s.value=t;
  redrawAll();
}

// ── Navigation ──────────────────────────────────────────────────────────────
function nav(v,fromHash){
  if(!VIEWS[v])v='overview';
  currentView=v;
  if(!fromHash){try{history.replaceState(null,'','#'+v);}catch(e){location.hash=v;}}
  document.querySelectorAll('.nav-item').forEach(function(n){
    n.classList.toggle('active',n.dataset.v===v);
  });
  document.querySelectorAll('.view').forEach(function(s){
    s.classList.toggle('on',s.id==='v-'+v);
  });
  set('pgT',VIEWS[v].t);set('pgS',VIEWS[v].s);
  el('ranges').style.visibility=(v==='overview'||v==='stats')?'visible':'hidden';
  document.body.classList.remove('nav-open');
  redrawAll();
  if(v==='fleet')fleetRefresh();
  if(v==='pools')renderPools();
  if(v==='workers')renderWorkerTiles();
}

// ── Chart engine ────────────────────────────────────────────────────────────
// Series come from GET /api/history when the firmware provides it (the proposed
// ring buffer); otherwise we fall back to what this tab has accumulated since
// it opened, and the badge says "client" so nobody mistakes it for real history.
function seriesFor(r){
  if(histCache[r]&&histCache[r].khs&&histCache[r].khs.length>1)return histCache[r];
  return {khs:hrBuf.slice(-MAX_HR),step:POLL/1000,client:true};
}
function loadHistory(r){
  if(hasDeviceHistory===false)return Promise.resolve();
  return fetchT('/api/history?range='+encodeURIComponent(r),3000)
    .then(function(x){if(!x.ok)throw 0;return x.json();})
    .then(function(j){
      if(!j||!Array.isArray(j.khs))throw 0;
      hasDeviceHistory=true;histCache[r]=j;redrawAll();
    })
    .catch(function(){if(hasDeviceHistory===null)hasDeviceHistory=false;});
}

function niceMax(v){
  if(v<=0)return 1;
  var e=Math.pow(10,Math.floor(Math.log10(v))),m=v/e;
  return (m<=1?1:m<=2?2:m<=5?5:10)*e;
}

// Draw an area+line chart into `svgId`. Returns the point geometry for tooltips.
var chartGeom={};
function drawChart(svgId,wrapId,data,ids){
  var svg=el(svgId),wrap=el(wrapId);
  if(!svg||!wrap)return;
  var W=wrap.clientWidth||620,H=wrap.clientHeight||240;
  svg.setAttribute('viewBox','0 0 '+W+' '+H);
  var padL=44,padB=22,padT=10;
  var cW=Math.max(10,W-padL-6),cH=Math.max(10,H-padB-padT);
  var acc=cvar('--acc')||'#22d3ee',pur=cvar('--pur')||'#8b5cf6',bd=cvar('--bd')||'#222';

  if(!data||data.length<2){
    svg.innerHTML='<text x="'+(W/2)+'" y="'+(H/2)+'" text-anchor="middle" fill="'+cvar('--mt')+
      '" font-size="12">Collecting samples…</text>';
    chartGeom[svgId]=null;
    ['N','A','Hi','Lo','V'].forEach(function(k){var e=el(ids[k]);if(e)e.textContent='—';});
    return;
  }
  var hi=Math.max.apply(null,data),lo=Math.min.apply(null,data);
  var top=niceMax(hi*1.08)||1;
  var x=function(i){return padL+(i/(data.length-1))*cW;};
  var y=function(v){return padT+cH-(v/top)*cH;};

  var g='<defs><linearGradient id="cg-'+svgId+'" x1="0" y1="0" x2="0" y2="1">'+
    '<stop offset="0%" stop-color="'+acc+'" stop-opacity=".34"/>'+
    '<stop offset="100%" stop-color="'+acc+'" stop-opacity="0"/></linearGradient>'+
    '<linearGradient id="sg-'+svgId+'" x1="0" y1="0" x2="1" y2="0">'+
    '<stop offset="0%" stop-color="'+pur+'"/><stop offset="100%" stop-color="'+acc+'"/></linearGradient></defs>';

  // horizontal grid + y labels
  for(var i=0;i<=4;i++){
    var gy=(padT+cH*i/4).toFixed(1), gv=top*(1-i/4);
    g+='<line x1="'+padL+'" x2="'+W+'" y1="'+gy+'" y2="'+gy+'" stroke="'+bd+
       '" stroke-width=".7" stroke-dasharray="3 5"/>';
    g+='<text x="'+(padL-8)+'" y="'+(+gy+3.5)+'" text-anchor="end" fill="'+cvar('--mt')+
       '" font-size="9.5">'+(gv>=1000?(gv/1000).toFixed(1)+'M':Math.round(gv))+'</text>';
  }
  // smooth path (Catmull-Rom -> cubic bezier)
  var p='M'+x(0).toFixed(1)+','+y(data[0]).toFixed(1);
  for(var j=0;j<data.length-1;j++){
    var p0=data[Math.max(0,j-1)],p1=data[j],p2=data[j+1],p3=data[Math.min(data.length-1,j+2)];
    var c1x=x(j)+(x(j+1)-x(j))/6, c1y=y(p1)+(y(p2)-y(p0))/6;
    var c2x=x(j+1)-(x(j+1)-x(j))/6, c2y=y(p2)-(y(p3)-y(p1))/6;
    p+='C'+c1x.toFixed(1)+','+c1y.toFixed(1)+' '+c2x.toFixed(1)+','+c2y.toFixed(1)+' '+
       x(j+1).toFixed(1)+','+y(p2).toFixed(1);
  }
  var base=padT+cH;
  g+='<path d="'+p+'L'+x(data.length-1).toFixed(1)+','+base+'L'+padL+','+base+'Z" fill="url(#cg-'+svgId+')"/>';
  g+='<path d="'+p+'" fill="none" stroke="url(#sg-'+svgId+')" stroke-width="2" stroke-linejoin="round" stroke-linecap="round"/>';
  g+='<circle id="'+svgId+'-cur" cx="'+x(data.length-1).toFixed(1)+'" cy="'+y(data[data.length-1]).toFixed(1)+
     '" r="3.5" fill="'+acc+'"><animate attributeName="r" values="3.5;6;3.5" dur="2s" repeatCount="indefinite"/></circle>';
  g+='<line id="'+svgId+'-hair" x1="0" x2="0" y1="'+padT+'" y2="'+base+'" stroke="'+acc+
     '" stroke-width="1" stroke-dasharray="3 3" opacity="0"/>';
  g+='<circle id="'+svgId+'-hd" r="4" fill="'+acc+'" opacity="0"/>';

  // x labels
  var s=seriesFor(range), step=s.step||300, n=data.length;
  for(var k=0;k<=4;k++){
    var idx=Math.round((n-1)*k/4);
    var secsAgo=(n-1-idx)*step;
    var lbl=secsAgo===0?'now':(secsAgo>=86400?Math.round(secsAgo/86400)+'d':
      secsAgo>=3600?Math.round(secsAgo/3600)+'h':Math.round(secsAgo/60)+'m');
    g+='<text x="'+x(idx).toFixed(1)+'" y="'+(H-6)+'" text-anchor="middle" fill="'+cvar('--mt')+
       '" font-size="9.5">'+lbl+'</text>';
  }
  svg.innerHTML=g;
  chartGeom[svgId]={data:data,x:x,y:y,padL:padL,cW:cW,step:step,n:n};

  var avg=data.reduce(function(a,b){return a+b;},0)/n;
  // "Current" must be the live sample, not the last history bucket — a 5-minute
  // bucket lags the KPI card by up to 5 minutes and reads as a bug.
  var cur=(lastStatus&&typeof lastStatus.hashrate_khs==='number')?lastStatus.hashrate_khs:data[n-1];
  set(ids.N,fmtH(cur));set(ids.A,fmtH(avg));
  set(ids.Hi,fmtH(hi));set(ids.Lo,fmtH(lo));
  set(ids.V,'±'+(avg?((hi-lo)/2/avg*100).toFixed(1):'0')+'%');
}

function bindTip(svgId,wrapId,tipId,tvId,ttId){
  var wrap=el(wrapId);if(!wrap)return;
  function move(ev){
    var G=chartGeom[svgId];if(!G)return;
    var r=wrap.getBoundingClientRect();
    var cx=(ev.touches?ev.touches[0].clientX:ev.clientX)-r.left;
    var i=Math.round((cx-G.padL)/G.cW*(G.n-1));
    if(i<0||i>=G.n){hide();return;}
    var tip=el(tipId),hair=el(svgId+'-hair'),hd=el(svgId+'-hd');
    var px=G.x(i),py=G.y(G.data[i]);
    if(hair){hair.setAttribute('x1',px);hair.setAttribute('x2',px);hair.setAttribute('opacity','.55');}
    if(hd){hd.setAttribute('cx',px);hd.setAttribute('cy',py);hd.setAttribute('opacity','1');}
    var secsAgo=(G.n-1-i)*G.step;
    el(tvId).textContent=fmtH(G.data[i]);
    el(ttId).textContent=secsAgo<1?'now':(secsAgo<3600?Math.round(secsAgo/60)+' min ago':
      secsAgo<86400?(secsAgo/3600).toFixed(1)+' h ago':(secsAgo/86400).toFixed(1)+' d ago');
    tip.style.left=px+'px';tip.style.top=py+'px';tip.style.opacity='1';
  }
  function hide(){
    var tip=el(tipId),hair=el(svgId+'-hair'),hd=el(svgId+'-hd');
    if(tip)tip.style.opacity='0';
    if(hair)hair.setAttribute('opacity','0');
    if(hd)hd.setAttribute('opacity','0');
  }
  wrap.addEventListener('mousemove',move);
  wrap.addEventListener('mouseleave',hide);
  wrap.addEventListener('touchmove',move,{passive:true});
  wrap.addEventListener('touchend',hide);
}

function renderCharts(){
  var s=seriesFor(range);
  histSrc=s.client?'client':'device';
  var cs=el('chartSrc');
  if(cs){
    cs.textContent=histSrc;
    cs.title=histSrc==='device'
      ? 'Series read from the miner’s on-device ring buffer'
      : 'This firmware has no history buffer — series accumulated in this tab since it opened, and lost on refresh';
  }
  if(currentView==='overview')drawChart('hrSvg','chartWrap',s.khs,{N:'csN',A:'csA',Hi:'csHi',Lo:'csLo',V:'csV'});
  if(currentView==='stats')drawChart('hrSvg2','chartWrap2',s.khs,{N:'s2N',A:'s2A',Hi:'s2Hi',Lo:'s2Lo',V:'s2V'});
}

// ── KPI trend delta (▲/▼ % vs earlier sample) ───────────────────────────────
function delta(id,arr){
  var e=el(id);if(!e||arr.length<3){if(e){e.textContent='—';e.className='delta fl';}return;}
  var a=arr[Math.max(0,arr.length-1-Math.min(arr.length-1,100))],b=arr[arr.length-1];
  if(!a){e.textContent='—';e.className='delta fl';return;}
  var p=(b-a)/a*100;
  var up=p>=0.05,dn=p<=-0.05;
  e.className='delta '+(up?'up':dn?'dn':'fl');
  e.textContent=(up?'▲ ':dn?'▼ ':'')+Math.abs(p).toFixed(1)+'%';
}

// ── Donut + gauge ───────────────────────────────────────────────────────────
function drawDonut(slices){
  var svg=el('donut');if(!svg)return;
  var C=2*Math.PI*15.9155, off=0, g='';
  if(!slices.length){
    svg.innerHTML='<circle cx="21" cy="21" r="15.9155" fill="none" stroke="'+cvar('--bd')+'" stroke-width="4"/>';
    set('dnN','0');el('poolLegend').innerHTML='<div class="hint">No reachable peers yet.</div>';
    return;
  }
  var tot=slices.reduce(function(a,s){return a+s.v;},0)||1;
  slices.forEach(function(s){
    var len=s.v/tot*C;
    g+='<circle cx="21" cy="21" r="15.9155" fill="none" stroke="'+s.c+'" stroke-width="4"'+
       ' stroke-dasharray="'+len.toFixed(2)+' '+(C-len).toFixed(2)+'"'+
       ' stroke-dashoffset="'+(-off).toFixed(2)+'" transform="rotate(-90 21 21)"'+
       ' style="transition:stroke-dasharray .6s"><title>'+esc(s.n)+'</title></circle>';
    off+=len;
  });
  svg.innerHTML=g;
  set('dnN',String(slices.length));
  el('poolLegend').innerHTML=slices.map(function(s){
    return '<div class="lg-i"><span class="lg-sw" style="background:'+s.c+'"></span>'+
      '<span class="lg-n">'+esc(s.n)+'</span><span class="lg-v">'+Math.round(s.v/tot*100)+'%</span></div>';
  }).join('');
}
function setGauge(score){
  var C=2*Math.PI*17;
  var arc=el('gArc');
  var col=score>=80?cvar('--grn'):score>=55?cvar('--gold'):cvar('--red');
  if(arc){arc.style.strokeDashoffset=String(C*(1-score/100));arc.style.stroke=col;}
  var n=el('gNum');if(n){n.textContent=score+'%';n.style.color=col;}
}
function redrawAll(){
  renderCharts();
  drawDonut(poolSlices());
}

// ── Insights (rule-based, derived from live telemetry only) ─────────────────
function insights(){
  var d=lastStatus,out=[];
  if(!d)return out;
  var heapP=d.total_heap?d.free_heap/d.total_heap*100:100;
  var tot=(d.shares||0)+rejected;
  var rejP=tot?rejected/tot*100:0;
  var rssi=d.wifi_rssi||0;

  if(hasTemp&&d.temp_c>=75)
    out.push({c:'var(--red)',k:'Temperature alert',t:'Die temperature is '+d.temp_c.toFixed(1)+'°C',
      d:'Sustained heat above 75 °C makes the ESP32 throttle its clock, which shows up as a slow hashrate decay. Improve airflow or drop CPU frequency.',m:null});
  else if(hasTemp&&d.temp_c>=65)
    out.push({c:'var(--gold)',k:'Thermal headroom',t:'Running warm at '+d.temp_c.toFixed(1)+'°C',
      d:'Still inside spec, but there is little margin left. Worth watching if the enclosure is sealed.',m:null});

  if(rejP>5&&tot>=20)
    out.push({c:'var(--red)',k:'Share quality',t:Math.round(rejP)+'% of shares rejected',
      d:'A reject rate this high usually means stale work: high latency to the pool, or a clock/difficulty mismatch. Try a closer pool.',m:null});

  if(heapP<15)
    out.push({c:'var(--gold)',k:'Memory pressure',t:'Only '+fmtB(d.free_heap)+' heap free',
      d:'Below ~15% free the HTTP server starts failing allocations before mining does. A restart reclaims fragmentation.',m:null});

  if(rssi&&rssi<-75)
    out.push({c:'var(--gold)',k:'Weak WiFi',t:'Signal at '+rssi+' dBm',
      d:'Marginal RSSI causes dropped stratum connections and stale shares. Move the miner or add an AP.',m:null});

  if(d.wg_enabled&&d.wg_state==='failed')
    out.push({c:'var(--red)',k:'VPN down',t:'WireGuard handshake failed',
      d:'The tunnel was torn down and direct routing restored, so mining continues. Check the endpoint port, the preshared key, and that the peer exists server-side.',m:null});
  else if(d.wg_enabled&&d.wg_state==='connecting')
    out.push({c:'var(--gold)',k:'VPN',t:'Handshaking…',
      d:'If this never resolves, the device clock may not have synced — WireGuard stamps a TAI64N timestamp from system time.',m:null});

  if(hrBuf.length>12){
    var recent=hrBuf.slice(-12);
    var avg=recent.reduce(function(a,b){return a+b;},0)/recent.length;
    var hi=Math.max.apply(null,recent),lo=Math.min.apply(null,recent);
    if(avg&&(hi-lo)/avg>0.25)
      out.push({c:'var(--gold)',k:'Stability',t:'Hashrate variance is '+Math.round((hi-lo)/avg*100)+'%',
        d:'Swings this wide are typically WiFi retransmits or thermal throttling rather than the SHA core itself.',m:null});
  }

  if(d.ota===false)
    out.push({c:'var(--blu)',k:'Maintenance',t:'No OTA partition on this board',
      d:'This firmware was built on a single-slot table (huge_app.csv), so updates must go over USB. Reflash with a two-slot table to enable OTA.',m:null});

  if(!out.length)
    out.push({c:'var(--grn)',k:'All clear',t:'Operating nominally',
      d:'Pool connected, memory and signal healthy, no rejected-share spike. Nothing needs attention.',
      m:tot?('Accept rate '+(100-rejP).toFixed(1)+'%'):null});
  return out.slice(0,4);
}
function renderInsights(){
  var c=el('insList');if(!c)return;
  c.innerHTML=insights().map(function(i){
    return '<div class="ins-i" style="--c:'+i.c+'">'+
      '<div class="ins-h"><svg class="i sm" viewBox="0 0 24 24"><use href="#i-spark"/></svg>'+esc(i.k)+'</div>'+
      '<div class="ins-t">'+esc(i.t)+'</div><div class="ins-d">'+esc(i.d)+'</div>'+
      (i.m?'<div class="ins-m">'+esc(i.m)+'</div>':'')+'</div>';
  }).join('');
}

// ── Activity feed ───────────────────────────────────────────────────────────
var EV={
  share_accepted:{i:'i-check',c:'var(--grn)',t:'Share accepted'},
  share_rejected:{i:'i-x',c:'var(--red)',t:'Share rejected'},
  pool_disconnect:{i:'i-warn',c:'var(--gold)',t:'Pool disconnected'},
  pool_connected:{i:'i-plug',c:'var(--grn)',t:'Pool connected'},
  block:{i:'i-block',c:'var(--gold)',t:'Block found'},
  info:{i:'i-act',c:'var(--acc)',t:'Event'}
};
function feedRow(e){
  var m=EV[e.kind]||EV.info;
  return '<div class="fe"><span class="ic" style="color:'+m.c+'"><svg class="i sm" viewBox="0 0 24 24"><use href="#'+m.i+'"/></svg></span>'+
    '<div class="fe-b"><div class="fe-t">'+esc(e.title||m.t)+'</div>'+
    '<div class="fe-d">'+esc(e.sub||'')+'</div></div>'+
    '<div class="fe-a">'+esc(e.when)+'</div></div>';
}
function emptyState(icon,title,sub){
  return '<div class="empty"><span class="ic"><svg class="i big" viewBox="0 0 24 24"><use href="#'+icon+'"/></svg></span>'+
    '<span class="et">'+esc(title)+'</span><span class="es">'+esc(sub)+'</span></div>';
}
function renderFeed(){
  var c=el('feed');if(!c)return;
  if(!alertLog.length){
    c.innerHTML=emptyState('i-inbox','No activity yet',
      'Shares, pool changes and warnings appear here as they happen. A NerdMiner submits a share every few seconds once the pool is subscribed.');
    return;
  }
  c.innerHTML=alertLog.slice(0,12).map(feedRow).join('');
}
function pushEvent(kind,title,sub){
  alertLog.unshift({kind:kind,title:title,sub:sub||'',when:nowT(),ts:Date.now()/1000});
  if(alertLog.length>40)alertLog.pop();
  renderFeed();renderAlerts();
  var p=el('navAlerts');
  var bad=alertLog.filter(function(a){return a.kind==='share_rejected'||a.kind==='pool_disconnect';}).length;
  if(p){p.style.display=bad?'':'none';p.textContent=String(bad);}
}
function renderAlerts(){
  var c=el('alertList');if(!c)return;
  set('alertCount',alertLog.length+' event'+(alertLog.length===1?'':'s'));
  c.innerHTML=alertLog.length?alertLog.map(feedRow).join('')
    :emptyState('i-check','No alerts','All systems operational. Rejected shares, pool drops and VPN failures are recorded here.');
}
function pushLog(msg,type){
  sysLog.unshift({t:nowT(),msg:msg,type:type||'inf'});
  if(sysLog.length>60)sysLog.pop();
  renderLogs();
}
function renderLogs(){
  var c=el('logList');if(!c)return;
  set('logCount',sysLog.length+' entr'+(sysLog.length===1?'y':'ies'));
  if(!sysLog.length){c.innerHTML=emptyState('i-term','No log entries yet','Client-side activity — pool switches, fleet syncs, scans — is recorded here for this browser session.');return;}
  var col={ok:'var(--grn)',err:'var(--red)',warn:'var(--gold)',inf:'var(--acc)'};
  c.innerHTML=sysLog.map(function(l){
    return '<div class="fe"><span class="ic" style="color:'+(col[l.type]||col.inf)+'">'+
      '<svg class="i sm" viewBox="0 0 24 24"><use href="#i-term"/></svg></span>'+
      '<div class="fe-b"><div class="fe-t" style="font-weight:500">'+esc(l.msg)+'</div></div>'+
      '<div class="fe-a">'+esc(l.t)+'</div></div>';
  }).join('');
}
// Per-share difficulty, newest last. NOT best_diff: that is a running maximum,
// so charting it produces a staircase that can only ever climb.
var shareDiffs=[];
function renderDbars(){
  var c=el('dbars');if(!c)return;
  var lbl=el('dbarsLbl');
  var src=shareDiffs.length?shareDiffs.slice(-7):null;
  if(!src){
    c.innerHTML='';
    if(lbl)lbl.textContent='Share difficulty — waiting for the first accepted share';
    return;
  }
  if(lbl)lbl.textContent='Share difficulty — last '+src.length+' share'+(src.length===1?'':'s');
  var mx=Math.max.apply(null,src)||1;
  c.innerHTML=src.map(function(v,i){
    return '<div class="dbar'+(i===src.length-1?' lat':'')+'" title="'+fmtD(v)+
      '" style="height:'+Math.max(3,Math.round(v/mx*32))+'px"></div>';
  }).join('');
}
// Called with no args on boot purely to paint the empty state.
function addShare(result,diff){
  if(result){
    var dv=parseFloat(diff)||0;
    if(dv>0){shareDiffs.push(dv);if(shareDiffs.length>32)shareDiffs.shift();renderDbars();}
    shareLog.unshift({t:nowT(),r:result,d:fmtD(diff)});
  }
  if(shareLog.length>20)shareLog.pop();
  set('shareCount',shareLog.length+' recent');
  var tb=el('sTbody');if(!tb)return;
  tb.innerHTML=shareLog.length?shareLog.map(function(s){
    return '<tr><td>'+esc(s.t)+'</td><td><span class="'+(s.r==='accepted'?'pa':'pj')+'">'+s.r+
      '</span></td><td class="vb">'+esc(s.d)+'</td></tr>';
  }).join(''):'<tr><td colspan="3">'+emptyState('i-coins','Waiting for shares…','Shares appear the moment the pool accepts one.')+'</td></tr>';
}

// ── Pool distribution (real, from fleet peers) ──────────────────────────────
function poolSlices(){
  var by={},cols=[cvar('--acc'),cvar('--pur'),cvar('--blu'),cvar('--gold'),cvar('--grn'),cvar('--red')];
  fleet.forEach(function(h){
    var r=fleetData[h];
    if(!r||!r.ok||!r.d)return;
    var k=(r.d.pool_url||'unknown');
    by[k]=(by[k]||0)+(r.d.hashrate_khs||0);
  });
  var keys=Object.keys(by);
  if(!keys.length&&lastStatus&&lastStatus.pool_url){by[lastStatus.pool_url]=lastStatus.hashrate_khs||1;keys=Object.keys(by);}
  return keys.sort(function(a,b){return by[b]-by[a];}).slice(0,6).map(function(k,i){
    return {n:k,v:by[k],c:cols[i%cols.length]};
  });
}
function renderPools(){
  var tb=el('poolBody');if(!tb)return;
  var cur=(lastStatus&&lastStatus.pool_url)||'';
  set('poolCount',knownPools.length+' known');
  tb.innerHTML=knownPools.map(function(p){
    var on=p.host===cur;
    return '<tr><td style="font-weight:600">'+esc(p.name||p.host)+'</td>'+
      '<td class="mono" style="color:var(--tx2)">'+esc(p.host)+'</td><td>'+esc(String(p.port))+'</td>'+
      '<td>'+(p.has_api?'<span class="vg">yes</span>':'<span style="color:var(--mt)">no</span>')+'</td>'+
      '<td>'+(on?'<span class="pa">active</span>':'<button class="btn" style="height:28px;padding:0 10px;font-size:11px" data-pool="'+esc(p.host+':'+p.port)+'">Switch</button>')+'</td></tr>';
  }).join('');
}
function renderWorkerTiles(){
  var c=el('wkTiles');if(!c||!lastStatus)return;
  var d=lastStatus,s=lastSys||{};
  var tiles=[
    ['i-act','Hashrate',fmtH(d.hashrate_khs||0),'--acc'],
    ['i-check','Accepted',fmtN(d.shares),'--grn'],
    ['i-x','Rejected',String(rejected),'--red'],
    ['i-coins','Best diff',fmtD(d.best_diff),'--pur'],
    ['i-clock','Uptime',fmtUp(d.uptime||0),'--blu'],
    ['i-cpu','Templates',fmtN(d.templates),'--acc'],
    ['i-mem','Free heap',fmtB(d.free_heap||0),'--pur'],
    ['i-wifi','RSSI',(d.wifi_rssi||0)+' dBm','--grn'],
    ['i-db','Total hashes',fmtN(d.total_mhashes)+' MH','--gold'],
    ['i-server','Pool',esc(d.pool_url||'—'),'--blu'],
    ['i-term','SSID',esc(d.wifi_ssid||'—'),'--acc'],
    ['i-shield','VPN',esc(d.wg_enabled?(d.wg_state||'—'):'off'),'--acc']
  ];
  if(hasTemp)tiles.splice(6,0,['i-therm','Temperature',d.temp_c.toFixed(1)+' °C','--gold']);
  c.innerHTML=tiles.map(function(t){
    return '<div class="ht"><span class="ic" style="background:color-mix(in srgb,var('+t[3]+') 14%,transparent);color:var('+t[3]+')">'+
      '<svg class="i sm" viewBox="0 0 24 24"><use href="#'+t[0]+'"/></svg></span>'+
      '<div class="ht-v" style="font-size:14px">'+t[2]+'</div><div class="ht-l">'+t[1]+'</div></div>';
  }).join('');
}

// ── Health score ────────────────────────────────────────────────────────────
function healthScore(d){
  if(!d)return 0;
  var s=0;
  s+=(d.pool_connected&&d.pool_subscribed)?45:0;
  if(!d.wg_enabled)s+=10; else s+=(d.wg_state==='up')?10:(d.wg_state==='connecting'?5:0);
  var r=d.wifi_rssi||-100;
  s+=r>=-60?15:r>=-70?11:r>=-80?6:2;
  var hp=d.total_heap?d.free_heap/d.total_heap:0;
  s+=hp>=.35?15:hp>=.25?11:hp>=.15?7:2;
  var tot=(d.shares||0)+rejected, rp=tot?rejected/tot:0;
  s+=tot<5?15:rp<=.02?15:rp<=.05?10:rp<=.10?5:0;
  return Math.max(0,Math.min(100,Math.round(s)));
}
function rssiQual(r){
  if(r>=-55)return ['Excellent','var(--grn)'];
  if(r>=-67)return ['Good','var(--grn)'];
  if(r>=-75)return ['Fair','var(--gold)'];
  return ['Weak','var(--red)'];
}

// ── Apply status ────────────────────────────────────────────────────────────
function applyStatus(d){
  var khs=d.hashrate_khs||0;
  // sparks.hash / sparks.temp back the KPI trend deltas (▲/▼ %); no drawn sparkline.
  push(hrBuf,khs,MAX_HR);push(sparks.hash,khs);
  hasTemp=(typeof d.temp_c==='number');
  el('kTempCard').style.display=hasTemp?'':'none';
  if(hasTemp)push(sparks.temp,d.temp_c);

  animNum('kHash',khs,function(v){return fmtHn(v)+' '+fmtHu(v);});
  delta('kHashD',sparks.hash);
  animNum('kAcc',d.shares||0,function(v){return fmtN(Math.round(v));});
  var tot=(d.shares||0)+rejected;
  set('kAccR',tot?((d.shares||0)/tot*100).toFixed(1)+'% accept rate':'— accept rate');
  set('kRej',String(rejected));
  set('kRejR',tot?(rejected/tot*100).toFixed(1)+'% of submissions':'this session');
  set('kUp',fmtUp(d.uptime||0));
  set('kUpS2','booted '+new Date(Date.now()-(d.uptime||0)*1000).toLocaleString());
  if(hasTemp){animNum('kTemp',d.temp_c,function(v){return v.toFixed(1)+'°C';});delta('kTempD',sparks.temp);}
  var fh=d.free_heap||0,th=d.total_heap||1;
  set('kHeap',fmtB(fh));set('kHeapS',Math.round(fh/th*100)+'% free of '+fmtB(th));
  set('hTpl',fmtN(d.templates));

  // events — the read-and-clear latches. Lossy by construction: another open
  // tab that polls first consumes them. /api/events fixes this when present.
  if(!hasEvents){
    if(d.ev_share_accepted){lastShare=nowT();addShare('accepted',d.best_diff);
      pushEvent('share_accepted','Share accepted','difficulty '+fmtD(d.best_diff));pushLog('Share accepted — diff '+fmtD(d.best_diff),'ok');}
    if(d.ev_share_rejected){rejected++;addShare('rejected',d.best_diff);
      pushEvent('share_rejected','Share rejected','best diff '+fmtD(d.best_diff));pushLog('Share rejected','err');}
    if(d.ev_pool_disconnect){pushEvent('pool_disconnect','Pool disconnected',d.pool_url||'');pushLog('Pool disconnected','warn');}
  }

  var fw=(d.firmware||'').replace(/^v/,'');
  set('verBadge','v'+(fw||'—'));
  set('otaV',d.firmware||'—');set('otaB',d.board||d.chip||'this board');

  var ok=d.pool_connected&&d.pool_subscribed;
  var badge=el('connBadge');
  if(badge){badge.className='badge '+(ok?'b-ok':'b-err');set('connTxt',ok?'Connected':d.pool_connected?'Syncing':'Disconnected');}
  if(prevConn!==null&&prevConn!==ok&&ok){pushEvent('pool_connected','Pool connected',d.pool_url||'');pushLog('Pool connected','ok');}
  prevConn=ok;

  var score=healthScore(d);
  setGauge(score);
  var hd=el('healthDot');
  var hc=score>=80?'var(--grn)':score>=55?'var(--gold)':'var(--red)';
  if(hd){hd.style.background=hc;hd.style.color=hc;}
  set('healthTxt',score>=80?'Healthy':score>=55?'Degraded':'Unhealthy');
  set('healthSub',ok?'All systems operational':'Pool not subscribed');
  set('tbHost',d.hostname||'NerdMiner');

  var vb=el('vpnBadge');
  if(vb){
    if(d.wg_enabled){
      var st=d.wg_state||(d.wg_connected?'up':'connecting');
      var tt={up:'WireGuard VPN — tunnel up',connecting:'WireGuard VPN — handshaking…',
        failed:'WireGuard VPN — no handshake; check keys, preshared key and port'};
      vb.style.display='';
      vb.className='badge '+(st==='up'?'b-vpn':st==='connecting'?'b-warn':'b-err');
      vb.title=tt[st]||('WireGuard VPN — '+st);
    }else vb.style.display='none';
  }

  var pu=d.pool_url||'';
  set('ppName',pu||'—');set('ppUrl',pu+(d.pool_port?':'+d.pool_port:''));
  var ppSt=el('ppSt');if(ppSt){ppSt.textContent=ok?'Connected':'Disconnected';ppSt.className='pr-v '+(ok?'vg':'vr');}
  set('ppLs',lastShare||'—');set('ppDf',fmtD(d.best_diff));
  // Stratum user is "<wallet>.<worker>". Printing the whole string in both rows
  // (as the old dashboard did) made them look like duplicated fields.
  var w=String(d.wallet||''),dot=w.lastIndexOf('.');
  set('ppWk',dot>0?w.slice(dot+1):'—');
  set('ppWa',trunc(dot>0?w.slice(0,dot):w,12));
  set('ppLat',latency==null?'—':latency+' ms');
  renderDbars();

  var sv=(d.pool_url||'')+':'+(d.pool_port||'');
  activePool=sv;
  var opt=el('poolSel').querySelector('option[value="'+sv+'"]');
  if(opt)el('poolSel').value=sv;

  set('hRssi',(d.wifi_rssi||0)+' dBm');
  var rq=rssiQual(d.wifi_rssi||-100);
  var rqe=el('hRssiQ');if(rqe){rqe.textContent=rq[0];rqe.style.color=rq[1];}
  set('hIp',d.ip||'—');set('hHost',d.hostname||'—');
  set('hOta',d.ota===false?'OTA unavailable':'OTA ready');
  set('hEff',tot?((d.shares||0)/tot*100).toFixed(1)+'%':'—');
  set('hLat',latency==null?'—':latency+' ms');

  lastWallet=d.wallet||lastWallet;
  selfIp=d.ip||selfIp;
  lastStatus=d;
  renderInsights();
  if(currentView==='workers')renderWorkerTiles();
  renderCharts();

  if(d.ip&&d.ip!=='0.0.0.0'&&!selfAdded){
    selfAdded=true;
    if(!fleetLoaded)fleetPendingSelf=d.ip;
    else if(fleet.indexOf(d.ip)<0){fleet.push(d.ip);fleetCache();fleetMutate([d.ip],[]);}
  }
  lastTs=new Date();updateFooter();
}

function applySystem(d){
  lastSys=d;
  set('hChip',d.chip_model||'—');
  set('hCpu',(d.cpu_freq_mhz||'—')+' MHz');
  set('hFlash',fmtB(d.flash_size||0));
  set('hMac',d.mac||'—');
  set('hSdk',(d.sdk_version||'—').substring(0,16));
  var used=(d.total_heap||1)-(d.free_heap||0);
  var pct=Math.round(used/(d.total_heap||1)*100);
  set('hMemP',pct+'%');
  var b=el('hMemB');if(b)b.style.width=pct+'%';
}
function updateFooter(){
  if(!lastTs)return;
  var s=Math.round((Date.now()-lastTs.getTime())/1000);
  set('ftL','Last updated '+s+'s ago');
  set('ftC','Timezone UTC'+(tz>=0?'+':'')+tz+' · Auto-refresh 3s · '+RANGE_LBL[range]+' window');
  set('ftR','Save stats to flash (NVS): '+(saveStats?'On':'Off'));
}

// ── Polling ─────────────────────────────────────────────────────────────────
var hasEvents=false;
function fetchStatus(){
  var t0=performance.now();
  return api('/api/status').then(function(r){
    if(!r.ok)throw new Error(r.status);
    latency=Math.round(performance.now()-t0);
    return r.json();
  }).then(applyStatus).catch(function(){
    var b=el('connBadge');if(b)b.className='badge b-err';
    set('connTxt','Offline');
  });
}
function fetchSystem(){
  return api('/api/system').then(function(r){if(!r.ok)throw 0;return r.json();}).then(applySystem).catch(function(){});
}
// Proposed /api/events: an append-only ring, so no browser can consume another's
// events. Falls back silently to the read-and-clear latches when absent.
function fetchEvents(){
  return fetchT('/api/events?since='+lastEvT,2500).then(function(r){if(!r.ok)throw 0;return r.json();})
    .then(function(list){
      if(!Array.isArray(list))throw 0;
      hasEvents=true;
      list.forEach(function(e){
        lastEvT=Math.max(lastEvT,e.t);
        if(e.kind==='share_rejected')rejected++;
        if(e.kind==='share_accepted'){lastShare=nowT();addShare('accepted',e.diff);}
        if(e.kind==='share_rejected')addShare('rejected',e.diff);
        alertLog.unshift({kind:e.kind,title:(EV[e.kind]||EV.info).t,
          sub:e.diff?('difficulty '+fmtD(e.diff)):'',when:ago(e.t),ts:e.t});
      });
      if(list.length){if(alertLog.length>40)alertLog.length=40;renderFeed();renderAlerts();}
    }).catch(function(){});
}
// ── Fleet ───────────────────────────────────────────────────────────────────
var knownPools=POOLS_FALLBACK.slice();
function fleetValid(h){return typeof h==='string'&&/^[A-Za-z0-9.\-]+(:\d{1,5})?$/.test(h);}
function fleetCache(){try{localStorage.setItem('nm_fleet',JSON.stringify(fleet));}catch(e){}}

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
    body.innerHTML='<tr><td colspan="9">'+emptyState('i-net','No miners yet',
      'Add one above, or hit Scan LAN. This device enrols itself automatically once it has an IP.')+'</td></tr>';
    set('flOnline','0');set('flHash',fmtH(0));set('flShares','0');
    drawDonut(poolSlices());
    return;
  }
  var online=0,totHash=0,totShares=0,pools={};
  var selfBuild=String((lastStatus&&lastStatus.build)||'');
  body.innerHTML=fleet.map(function(host){
    var res=fleetData[host];
    var rm='<td style="text-align:right"><button class="lnk-x" title="Remove" data-host="'+esc(host)+'">✕</button></td>';
    if(res&&res.ok&&res.d){
      var d=res.d;online++;
      var khs=d.hashrate_khs||0;totHash+=khs;totShares+=(d.shares||0);
      if(d.pool_url)pools[d.pool_url]=1;
      var name=d.hostname||host,conn=d.pool_connected&&d.pool_subscribed;
      var b=String(d.build||d.firmware||''),bx=String(d.build||'');
      var ver=esc(b||'—')+(selfBuild&&bx&&bx!==selfBuild?' <span class="pj" title="differs from this device">≠</span>':'');
      return '<tr><td><a href="http://'+esc(host)+'/" target="_blank" rel="noopener" class="fl-link">'+esc(name)+
        (res.self?' <span style="color:var(--mt)">(this)</span>':'')+'</a></td>'+
        '<td style="color:var(--mt)" class="mono">'+esc(d.ip||host)+'</td>'+
        '<td><span class="'+(conn?'pa':'pj')+'">'+(conn?'mining':esc(d.status||'sync'))+'</span></td>'+
        '<td>'+fmtH(khs)+'</td><td>'+fmtN(d.shares)+'</td><td class="vb">'+fmtD(d.best_diff)+'</td>'+
        '<td style="color:var(--mt)">'+fmtUp(d.uptime||0)+'</td>'+
        '<td style="color:var(--mt)" class="mono">'+ver+'</td>'+rm+'</tr>';
    }
    var st=res?'<span class="pj">offline</span>':'<span style="color:var(--mt)">polling…</span>';
    return '<tr><td>'+esc(host)+'</td><td style="color:var(--mt)">—</td><td>'+st+'</td>'+
      '<td colspan="5" style="color:var(--mt)">—</td>'+rm+'</tr>';
  }).join('');

  set('flOnline',String(online));set('flHash',fmtH(totHash));set('flShares',fmtN(totShares));
  drawDonut(poolSlices());
}
function fleetRemove(host){
  delete fleetData[host];
  fleet=fleet.filter(function(h){return h!==host;});
  fleetCache();renderFleet();fleetMutate([],[host]);
}
function fleetAdd(host){
  host=(host||'').trim().replace(/^https?:\/\//,'').replace(/\/.*$/,'');
  if(!host){toast('Enter an IP or hostname','err');return;}
  if(!fleetValid(host)){toast('Invalid IP or hostname','err');return;}
  if(fleet.indexOf(host)>=0){toast('Already in fleet','warn');el('fleetInput').value='';return;}
  if(fleet.length>=FLEET_MAX){toast('Fleet limit ('+FLEET_MAX+') reached','err');return;}
  fleet.push(host);fleetCache();renderFleet();fleetRefresh();fleetMutate([host],[]);
}
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
function deriveSubnet(){
  var ip=(lastStatus&&lastStatus.ip)||selfIp||'';
  if(!/^\d+\.\d+\.\d+\.\d+$/.test(ip))return null;
  return ip.split('.').slice(0,3).join('.');
}
function scanProbe(ip){
  return fetchT('http://'+ip+'/api/status',1200)
    .then(function(r){if(!r.ok)throw 0;return r.json();})
    .then(function(d){return (d&&typeof d.hashrate_khs!=='undefined'&&typeof d.pool_subscribed!=='undefined')?ip:null;})
    .catch(function(){return null;});
}
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
    var CONC=24;
    for(var s=0;s<ips.length;s+=CONC){
      var res=await Promise.all(ips.slice(s,s+CONC).map(scanProbe));
      res.forEach(function(ip){if(ip)consider(ip);});
      if(btn)btn.textContent='Scanning… '+Math.min(ips.length,s+CONC)+'/254';
    }
  }
  var added=newHosts.length;
  if(added){fleet=fleet.concat(newHosts);fleetCache();fleetMutate(newHosts,[]);}
  if(btn){btn.disabled=false;btn.textContent='Scan LAN';}
  scanning=false;
  toast('Scan complete — '+found+' miner'+(found===1?'':'s')+' found'+(added?(', '+added+' new'):'')+(viaMdns?' via mDNS':''),found?'ok':'warn');
  pushLog((viaMdns?'mDNS discovery':'LAN scan')+': '+found+' miner(s) found','ok');
  renderFleet();fleetRefresh();
}
function isSelfHost(h){
  h=String(h||'').toLowerCase();
  if(!h)return false;
  if(selfIp&&selfIp!=='0.0.0.0'&&(h===selfIp||h===selfIp+':80'))return true;
  var hn=String((lastStatus&&lastStatus.hostname)||'').toLowerCase();
  return !!hn&&(h===hn||h===hn+'.local'||h===hn+'.local:80');
}
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
      .then(function(r){if(r.ok)ok++;else fail++;}).catch(function(){fail++;});
  }));
  var msg='Restarted '+ok+' of '+others.length+' peer'+(others.length===1?'':'s')+(fail?(' ('+fail+' failed)'):'');
  pushLog('Fleet restart: '+msg,fail?'warn':'ok');
  if(self){try{await api('/api/restart',{method:'POST'});msg+=' — this device restarting…';}catch(e){}}
  toast(msg,fail?'warn':'ok');
  if(btn)btn.disabled=false;
}

// ── OTA ─────────────────────────────────────────────────────────────────────
function fwValid(f){
  if(!/\.bin$/i.test(f.name)){toast('Select a firmware .bin file','err');return false;}
  if(f.size<102400){toast('That file is too small to be NerdMiner firmware','err');return false;}
  if(/factory/i.test(f.name)){toast('Use the bare app image (firmware.bin), not *_factory.bin','err');return false;}
  return true;
}
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
async function startOta(){
  var fi=el('otaFile');
  if(!fi.files.length){toast('Select a .bin file first','err');return;}
  var file=fi.files[0];
  if(!fwValid(file))return;
  if(!confirm('Flash '+file.name+' ('+fmtB(file.size)+')?'))return;
  var w=el('otaW'),fill=el('otaFill'),msg=el('otaMsg');
  w.style.display='block';
  otaBusy=true;
  try{
    var res=await otaUpload('/api/ota',file,function(p,loaded,total){
      fill.style.width=p+'%';
      msg.textContent='Uploading… '+fmtB(loaded)+' / '+fmtB(total);
    });
    if(res.ok){msg.textContent='Flash complete! Restarting…';toast('OTA complete','ok');}
    else{msg.textContent='Failed: '+res.err;toast('OTA failed: '+res.err,'err');}
  }finally{otaBusy=false;}
}

// ── Config ──────────────────────────────────────────────────────────────────
async function loadPools(){
  try{var r=await api('/api/pools');if(r.ok){var j=await r.json();if(Array.isArray(j)&&j.length)knownPools=j;}}catch(e){}
  var sel=el('poolSel');if(!sel)return;
  sel.innerHTML='<option value="">— Switch pool —</option>'+knownPools.map(function(p){
    return '<option value="'+esc(p.host+':'+p.port)+'">'+esc(p.name||p.host)+'</option>';
  }).join('');
  if(activePool)sel.value=activePool;
  renderPools();
}
async function switchPool(v){
  if(!v)return;
  var i=v.lastIndexOf(':'),host=v.slice(0,i),port=parseInt(v.slice(i+1))||3333;
  if(!confirm('Switch this miner to '+host+':'+port+'?\n\nThe new pool is saved and the device restarts.')){
    el('poolSel').value=activePool||'';return;
  }
  toast('Switching to '+host+'…','warn');
  try{
    var cfgR=await api('/api/config');var cfg=cfgR.ok?await cfgR.json():{};
    var wallet=((cfg.wallet||lastWallet||'')+'').trim();
    if(!wallet){toast('No wallet set — open Settings first','err');el('poolSel').value=activePool||'';return;}
    var body={wallet:wallet,pool_url:host,pool_port:port,pool_pass:cfg.pool_pass||'x',
      timezone:(cfg.timezone!=null?cfg.timezone:tz),save_stats:!!cfg.save_stats};
    var r=await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
    var d=await r.json();
    if(d.success){toast('Pool switched — device restarting…','ok');pushLog('Pool switched to '+host+':'+port,'ok');}
    else{toast('Switch failed: '+(d.error||'unknown'),'err');el('poolSel').value=activePool||'';}
  }catch(e){toast('Switch error: '+e.message,'err');el('poolSel').value=activePool||'';}
}
async function loadCfg(){
  el('cToken').value=apiToken;
  el('cThemeSel').value=currentTheme();
  try{
    var r=await api('/api/config');if(!r.ok)return;
    var d=await r.json();
    el('cWallet').value=d.wallet||'';el('cUrl').value=d.pool_url||'';
    el('cPort').value=d.pool_port||'';el('cPass').value=d.pool_pass||'';
    el('cTz').value=d.timezone||0;el('cSave').checked=!!d.save_stats;
    tz=d.timezone||0;saveStats=!!d.save_stats;
    el('cAlUrl').value=d.alert_url||'';
    if(d.alert_service)el('cAlSvc').value=d.alert_service;
    if('wg_enabled' in d){
      el('wgSection').style.display='block';
      el('cWgEn').checked=!!d.wg_enabled;
      el('cWgIp').value=d.wg_local_ip||'';
      el('cWgEp').value=d.wg_endpoint||'';
      el('cWgPort').value=d.wg_port||51820;
      el('cWgPub').value=d.wg_peer_public_key||'';
      el('cWgPriv').value='';el('cWgPsk').value='';el('cWgPskClear').checked=false;
      set('cWgPkState',d.wg_has_privkey?'(one is set — blank keeps it)':'(none set yet)');
      set('cWgPskState',d.wg_has_psk?'(one is set — blank keeps it)':'(none set — omit unless the server sends one)');
    }else el('wgSection').style.display='none';
  }catch(e){}
}
async function saveCfg(){
  var body={wallet:el('cWallet').value.trim(),pool_url:el('cUrl').value.trim(),
    pool_port:parseInt(el('cPort').value)||21496,pool_pass:el('cPass').value.trim(),
    timezone:parseInt(el('cTz').value)||0,save_stats:el('cSave').checked,
    alert_url:el('cAlUrl').value.trim(),alert_service:el('cAlSvc').value};
  if(!body.wallet){toast('Wallet address required','err');return;}
  if(!body.pool_url){toast('Pool URL required','err');return;}
  if(el('wgSection').style.display!=='none'){
    var wgEn=el('cWgEn').checked;
    var wgIp=el('cWgIp').value.trim(),wgEp=el('cWgEp').value.trim();
    var wgPub=el('cWgPub').value.trim(),wgPriv=el('cWgPriv').value.trim();
    var wgPsk=el('cWgPsk').value.trim(),wgPskClear=el('cWgPskClear').checked;
    if(wgEn&&(!wgIp||!wgEp||!wgPub)){toast('VPN needs tunnel IP, endpoint and server public key','err');return;}
    if(wgEn&&!/^\d+\.\d+\.\d+\.\d+$/.test(wgIp)){toast('Tunnel IP must be an IPv4 address','err');return;}
    if(wgPsk&&wgPsk.length!==44){toast('Preshared key must be 44 base64 characters','err');return;}
    if(wgPsk&&wgPskClear){toast('Either paste a preshared key or tick Remove — not both','err');return;}
    body.wg_enabled=wgEn;body.wg_local_ip=wgIp;body.wg_endpoint=wgEp;
    body.wg_port=parseInt(el('cWgPort').value)||51820;
    body.wg_peer_public_key=wgPub;
    if(wgPriv)body.wg_private_key=wgPriv;
    if(wgPsk)body.wg_preshared_key=wgPsk;
    if(wgPskClear)body.wg_clear_psk=true;
  }
  set('cMsg','Saving…');
  try{
    var r=await api('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
    var d=await r.json();
    if(d.success){toast('Saved — restarting…','ok');set('cMsg','Saved. Device restarting…');}
    else{toast('Error: '+(d.error||'unknown'),'err');set('cMsg','Error: '+(d.error||''));}
  }catch(e){toast('Error: '+e.message,'err');}
}
async function alertTest(){
  var url=el('cAlUrl').value.trim();
  if(!url){toast('Enter a webhook URL first','err');return;}
  toast('Sending test alert…','ok');
  try{
    var r=await api('/api/alert/test',{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({url:url,service:el('cAlSvc').value})});
    var d=await r.json();
    toast(d.success?'Test sent — check your webhook':'Failed: '+(d.error||('HTTP '+r.status)),d.success?'ok':'err');
  }catch(e){toast('Test failed: '+e.message,'err');}
}
async function testPool(){
  toast('Testing connection…','ok');
  try{var r=await api('/api/pool/test');var d=await r.json();
    toast(d.success?'Pool reachable':'Pool unreachable: '+d.message,d.success?'ok':'err');}
  catch(e){toast('Test failed: '+e.message,'err');}
}
async function restartDevice(){
  if(!confirm('Restart the miner now?'))return;
  try{await api('/api/restart',{method:'POST'});toast('Restarting…','warn');}catch(e){toast('Restart sent','warn');}
}
async function factoryReset(){
  if(!confirm('Factory reset will erase ALL settings. Are you sure?'))return;
  if(!confirm('This cannot be undone. Confirm?'))return;
  try{await api('/api/reset',{method:'POST'});toast('Factory reset — restarting…','warn');}catch(e){toast('Reset sent','warn');}
}
// Config backup: fetch /api/config and save it. Secrets are already write-only
// server-side (only wg_has_* booleans come back), so nothing sensitive lands on disk.
async function backupCfg(){
  try{
    var r=await api('/api/config');
    if(!r.ok)throw new Error('HTTP '+r.status);
    var j=await r.json();
    var name='nerdminer-'+((lastStatus&&lastStatus.hostname)||'config')+'.json';
    var blob=new Blob([JSON.stringify(j,null,2)],{type:'application/json'});
    var a=document.createElement('a');
    a.href=URL.createObjectURL(blob);a.download=name;a.click();
    setTimeout(function(){URL.revokeObjectURL(a.href);},4000);
    toast('Config downloaded (keys are never exported)','ok');
    pushLog('Config backed up to '+name,'ok');
  }catch(e){toast('Backup failed: '+e.message,'err');}
}
function openCfg(){loadCfg();set('cMsg','');el('moCfg').classList.add('show');}
function openOta(){
  if(lastStatus&&lastStatus.ota===false){toast('This board has no OTA partition — flash it over USB','err');return;}
  el('moOta').classList.add('show');
}
function closeM(id){el(id).classList.remove('show');}

// ── Wiring ──────────────────────────────────────────────────────────────────
var ACT={cfg:openCfg,ota:openOta,test:testPool,restart:restartDevice,reset:factoryReset,
  backup:backupCfg,save:saveCfg,alertTest:alertTest,otaStart:startOta,
  fleetAdd:function(){fleetAdd(el('fleetInput').value);el('fleetInput').value='';},
  fleetScan:fleetScan,fleetRefresh:fleetRefresh,fleetRestart:fleetRestartAll};

document.addEventListener('click',function(e){
  var t=e.target.closest?e.target.closest('[data-act],[data-v],[data-close],[data-theme],[data-host],[data-pool]'):null;
  if(!t)return;
  if(t.dataset.act&&ACT[t.dataset.act]){ACT[t.dataset.act]();closeDd();return;}
  if(t.dataset.theme){setTheme(t.dataset.theme);closeDd();return;}
  if(t.dataset.close){closeM(t.dataset.close);return;}
  if(t.dataset.host){fleetRemove(t.dataset.host);return;}
  if(t.dataset.pool){switchPool(t.dataset.pool);return;}
  if(t.dataset.v){nav(t.dataset.v);return;}
});
document.addEventListener('keydown',function(e){
  if(e.key==='Escape'){document.querySelectorAll('.mo.show').forEach(function(m){m.classList.remove('show');});closeDd();}
  if(e.key==='\\'&&!/^(INPUT|TEXTAREA)$/.test(document.activeElement.tagName))toggleRail();
  if(e.key==='Enter'&&document.activeElement&&document.activeElement.classList.contains('nav-item'))
    document.activeElement.click();
});
el('fleetInput').addEventListener('keydown',function(e){if(e.key==='Enter')ACT.fleetAdd();});
el('poolSel').addEventListener('change',function(){switchPool(this.value);});
el('cThemeSel').addEventListener('change',function(){setTheme(this.value);});
el('cToken').addEventListener('change',function(){setToken(this.value);});
document.querySelectorAll('.mo').forEach(function(m){
  m.addEventListener('click',function(e){if(e.target===m)m.classList.remove('show');});
});
document.querySelectorAll('.rg').forEach(function(b){
  b.addEventListener('click',function(){
    document.querySelectorAll('.rg').forEach(function(x){x.classList.remove('active');x.removeAttribute('aria-selected');});
    b.classList.add('active');b.setAttribute('aria-selected','true');
    range=b.dataset.r;
    if(hasDeviceHistory!==false&&!histCache[range])loadHistory(range);
    renderCharts();updateFooter();
  });
});
function closeDd(){el('ddTheme').classList.remove('open');}
el('btnTheme').addEventListener('click',function(e){e.stopPropagation();el('ddTheme').classList.toggle('open');});
document.addEventListener('click',function(e){if(!el('ddTheme').contains(e.target))closeDd();});
function toggleRail(){
  document.body.classList.toggle('rail');
  try{localStorage.setItem('nm_rail',document.body.classList.contains('rail')?'1':'0');}catch(e){}
  setTimeout(redrawAll,240);
}
el('btnRail').addEventListener('click',toggleRail);
el('btnMenu').addEventListener('click',function(){document.body.classList.toggle('nav-open');});
el('scrim').addEventListener('click',function(){document.body.classList.remove('nav-open');});

var rTmr;
window.addEventListener('resize',function(){clearTimeout(rTmr);rTmr=setTimeout(redrawAll,110);});

bindTip('hrSvg','chartWrap','tip','tipV','tipT');
bindTip('hrSvg2','chartWrap2','tip2','tipV2','tipT2');

window.addEventListener('hashchange',function(){nav((location.hash||'').replace('#',''),true);});

// ── Boot ────────────────────────────────────────────────────────────────────
setTheme(currentTheme());
var boot=(location.hash||'').replace('#','');
if(VIEWS[boot])nav(boot,true);
renderFeed();renderAlerts();renderLogs();addShare();drawDonut(poolSlices());
pushLog('Dashboard connected','ok');
fleetLoad();

// Probe /api/events BEFORE the first status poll, so we never double-count a
// share through both the event ring and the read-and-clear latches.
fetchEvents().then(function(){
  fetchStatus();
  setInterval(fetchStatus,POLL);
  if(hasEvents)setInterval(fetchEvents,POLL);
});
fetchSystem();loadCfg();loadPools();
loadHistory(range);
setInterval(fetchSystem,30000);
setInterval(updateFooter,1000);
setInterval(function(){if(currentView==='fleet'||currentView==='overview')fleetRefresh();},5000);
setInterval(function(){if(hasDeviceHistory)loadHistory(range);},60000);

})();
</script>
</body>
</html>)rawdash";
