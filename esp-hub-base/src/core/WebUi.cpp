/**
 * @file WebUi.cpp
 * @brief Web routes, status JSON, SSE, and upload OTA handlers.
 *
 * Base repository (keep when forking/copying):
 * https://github.com/MPunktBPunkt/ESP32.esp-hub
 */

#include "WebUi.h"
#include "IoTypes.h"
#include "Checkpoint.h"
#include "DiagLog.h"
#include "DiagSaved.h"
#include "PanicTail.h"

#include <Update.h>
#include <WiFi.h>

// ── PROGMEM HTML ─────────────────────────────────────────────────

static const char PAGE_CSS[] PROGMEM = R"HTML(
*{box-sizing:border-box;margin:0;padding:0}
body{background:#0d1117;color:#e6edf3;font-family:sans-serif;font-size:14px}
header{background:#161b22;border-bottom:1px solid #30363d;padding:14px 20px;display:flex;align-items:center;gap:12px}
h1{font-size:18px;color:#58a6ff}
.badge{background:rgba(88,166,255,.15);color:#58a6ff;border:1px solid rgba(88,166,255,.3);padding:2px 8px;border-radius:10px;font-size:11px}
.dot{width:8px;height:8px;border-radius:50%;display:inline-block;margin-right:6px}
.dot-on{background:#3fb950;box-shadow:0 0 6px #3fb950}.dot-off{background:#f85149}
.tabs{display:flex;background:#161b22;border-bottom:1px solid #30363d;padding:0 20px}
.tab{padding:10px 20px;border-bottom:2px solid transparent;color:#8b949e;text-decoration:none;font-weight:500}
.tab.active,.tab:hover{color:#58a6ff;border-bottom-color:#58a6ff}
.content{padding:20px;max-width:800px}
.card{background:#161b22;border:1px solid #30363d;border-radius:8px;padding:16px;margin-bottom:14px}
.card h3{font-size:11px;color:#8b949e;text-transform:uppercase;letter-spacing:.5px;margin-bottom:12px}
table{width:100%;border-collapse:collapse;font-size:13px}
td{padding:7px 10px;border-bottom:1px solid #21262d}
td:first-child{color:#8b949e;width:40%}
.bar{height:6px;background:#21262d;border-radius:3px;margin-top:4px;overflow:hidden}
.bar-fill{height:100%;border-radius:3px;transition:width .5s}
.green{background:#3fb950}.yellow{background:#e3b341}.red{background:#f85149}
.mono{font-family:monospace;color:#58a6ff}
.io-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(160px,1fr));gap:8px}
.io-card{background:#1c2128;border:1px solid #30363d;border-radius:6px;padding:10px;text-align:center}
.io-val{font-size:22px;font-weight:700;color:#58a6ff}
.io-key{font-size:11px;color:#8b949e;margin-top:4px}
)HTML";

static const char PAGE_BODY[] PROGMEM = R"HTML(
<header>
  <h1>&#128225; <span id="h-name"></span></h1>
  <span class="badge" id="h-ver"></span>
  <span style="margin-left:auto;font-size:12px;color:#8b949e">
    <span class="dot dot-off" id="sse-dot"></span>
    <span id="sse-lbl">Verbinde...</span>
  </span>
</header>
<div class="tabs">
  <a class="tab active" href="/">&#128200; Status</a>
  <a class="tab" href="/ota">&#128640; OTA Update</a>
</div>
<div class="content">
  <div class="card"><h3>Geraet</h3><table>
    <tr><td>Name</td><td id="d-name">-</td></tr>
    <tr><td>Chip</td><td id="d-chip">-</td></tr>
    <tr><td>MAC</td><td class="mono" id="d-mac">-</td></tr>
    <tr><td>IP-Adresse</td><td class="mono" id="d-ip">-</td></tr>
    <tr><td>WLAN RSSI</td><td id="d-rssi">-</td></tr>
    <tr><td>Uptime</td><td id="d-uptime">-</td></tr>
    <tr><td>Firmware</td><td id="d-ver">-</td></tr>
  </table></div>
  <div class="card"><h3>Speicher</h3><table>
    <tr><td>Freier RAM</td><td>
      <span id="d-heap">-</span>
      <div class="bar"><div class="bar-fill" id="b-heap"></div></div>
    </td></tr>
    <tr id="row-psram" style="display:none"><td>PSRAM frei</td><td>
      <span id="d-psram">-</span>
      <div class="bar"><div class="bar-fill" id="b-psram"></div></div>
    </td></tr>
    <tr><td>Freier Flash (OTA)</td><td>
      <span id="d-sketch">-</span>
      <div class="bar"><div class="bar-fill" id="b-sketch"></div></div>
    </td></tr>
  </table></div>
  <div class="card" id="io-card" style="display:none">
    <h3>IO-Werte</h3>
    <div class="io-grid" id="io-grid"></div>
  </div>
  <div class="card"><h3>ESP-Hub</h3><table>
    <tr><td>Hub-Adresse</td><td class="mono" id="d-hub">-</td></tr>
    <tr><td>Heartbeat</td><td id="d-interval">-</td></tr>
  </table></div>
</div>
<script>
function fmtKB(b){return Math.round(b/1024)+' KB';}
function barColor(p){return p>40?'green':p>20?'yellow':'red';}
function applyStatus(s){
  document.getElementById('h-name').textContent=s.name||'';
  document.getElementById('h-ver').textContent='v)HTML";

static const char PAGE_SCRIPT[] PROGMEM = R"HTML(';
  document.getElementById('d-name').textContent=s.name||'';
  document.getElementById('d-chip').textContent=s.chip||'';
  document.getElementById('d-mac').textContent=s.mac||'';
  document.getElementById('d-ip').innerHTML='<a href="http://'+s.ip+'" style="color:#58a6ff">'+s.ip+'</a>';
  document.getElementById('d-rssi').textContent=(s.rssi||0)+' dBm';
  document.getElementById('d-uptime').textContent=s.uptime||'';
  document.getElementById('d-ver').textContent='v)HTML";

static const char PAGE_SCRIPT2[] PROGMEM = R"HTML(';
  document.getElementById('d-hub').textContent=s.hub||'';
  document.getElementById('d-interval').textContent='alle '+(s.interval||30)+' Sekunden';
  var hp=s.heap||0,hpct=Math.min(100,Math.round(hp/3276));
  document.getElementById('d-heap').textContent=fmtKB(hp);
  var bh=document.getElementById('b-heap');bh.style.width=hpct+'%';bh.className='bar-fill '+barColor(hpct);
  var rowPs=document.getElementById('row-psram');
  var psz=s.psramSize||0,psf=s.psram||0;
  if(psz>0){
    rowPs.style.display='';
    var ppct=Math.min(100,Math.round(psf/(psz/100)));
    document.getElementById('d-psram').textContent=fmtKB(psf)+' / '+fmtKB(psz);
    var bp=document.getElementById('b-psram');bp.style.width=ppct+'%';bp.className='bar-fill '+barColor(ppct);
  }else{rowPs.style.display='none';}
  var sk=s.sketch||0,spct=Math.min(100,Math.round(sk/19660));
  document.getElementById('d-sketch').textContent=fmtKB(sk);
  var bs=document.getElementById('b-sketch');bs.style.width=spct+'%';bs.className='bar-fill '+barColor(spct);
  var ios=s.ios||{},keys=Object.keys(ios);
  var card=document.getElementById('io-card'),grid=document.getElementById('io-grid');
  if(keys.length>0){
    card.style.display='';
    grid.innerHTML=keys.map(function(k){
      var io=ios[k];
      return '<div class="io-card"><div class="io-val">'+parseFloat(io.v).toFixed(1)+(io.u?' '+io.u:'')+
             '</div><div class="io-key">'+k+'</div></div>';
    }).join('');
  }
}
var es=new EventSource('/events');
var dot=document.getElementById('sse-dot'),lbl=document.getElementById('sse-lbl');
es.onopen=function(){dot.className='dot dot-on';lbl.textContent='Live';};
es.onerror=function(){dot.className='dot dot-off';lbl.textContent='Getrennt';setTimeout(pollStatus,3000);};
es.onmessage=function(e){try{applyStatus(JSON.parse(e.data));}catch(ex){}};
function pollStatus(){
  fetch('/api/status').then(function(r){return r.json();}).then(applyStatus).catch(function(){});
}
pollStatus();
</script></body></html>)HTML";

static const char OTA_PAGE[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="de"><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>OTA Update</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#0d1117;color:#e6edf3;font-family:sans-serif;font-size:14px}
header{background:#161b22;border-bottom:1px solid #30363d;padding:14px 20px;display:flex;align-items:center;gap:12px}
h1{font-size:18px;color:#58a6ff}
.badge{background:rgba(88,166,255,.15);color:#58a6ff;border:1px solid rgba(88,166,255,.3);padding:2px 8px;border-radius:10px;font-size:11px}
.tabs{display:flex;background:#161b22;border-bottom:1px solid #30363d;padding:0 20px}
.tab{padding:10px 20px;border-bottom:2px solid transparent;color:#8b949e;text-decoration:none;font-weight:500}
.tab.active,.tab:hover{color:#58a6ff;border-bottom-color:#58a6ff}
.content{padding:20px;max-width:800px}
.card{background:#161b22;border:1px solid #30363d;border-radius:8px;padding:20px;margin-bottom:14px}
.drop{border:2px dashed #30363d;border-radius:8px;padding:32px;text-align:center;cursor:pointer;color:#8b949e}
.drop:hover{border-color:#58a6ff;color:#e6edf3}
input[type=file]{display:none}
.btn{background:#238636;color:#fff;border:none;padding:10px 24px;border-radius:6px;cursor:pointer;font-size:14px;font-weight:600;margin-top:14px;width:100%}
.btn:hover{background:#2ea043}
.prog{display:none;margin-top:14px}
.bar{height:8px;background:#21262d;border-radius:4px;overflow:hidden}
.bar-fill{height:100%;background:#58a6ff;border-radius:4px;width:0%;transition:width .3s}
.msg{margin-top:10px;font-size:13px;color:#8b949e;text-align:center}
.warn{background:rgba(227,179,65,.1);border:1px solid rgba(227,179,65,.3);border-radius:6px;padding:12px;color:#e3b341;font-size:13px;margin-bottom:14px}
</style></head><body>
<header><h1>&#128225; )HTML";

static const char OTA_SCRIPT[] PROGMEM = R"HTML(</h1><span class="badge">v)HTML";

static const char OTA_SCRIPT2[] PROGMEM = R"HTML(</span></header>
<div class="tabs">
  <a class="tab" href="/">&#128200; Status</a>
  <a class="tab active" href="/ota">&#128640; OTA Update</a>
</div>
<div class="content"><div class="card">
  <h3>&#128640; Firmware hochladen</h3>
  <div class="warn">&#9888; Nach dem Hochladen startet der ESP neu.</div>
  <div class="drop" id="drop" onclick="document.getElementById('fw').click()">
    &#128190; <b>.bin</b> Datei hierher ziehen oder klicken
  </div>
  <input type="file" id="fw" accept=".bin">
  <div id="fname" style="margin-top:8px;font-size:12px;color:#8b949e;text-align:center"></div>
  <button class="btn" onclick="startUpload()">&#9889; Flashen</button>
  <div class="prog" id="prog">
    <div class="bar"><div class="bar-fill" id="bar"></div></div>
    <div class="msg" id="msg">Lade hoch...</div>
  </div>
</div></div>
<script>
var inp=document.getElementById('fw'),drop=document.getElementById('drop');
inp.addEventListener('change',function(){
  if(inp.files[0])document.getElementById('fname').textContent=inp.files[0].name+' ('+Math.round(inp.files[0].size/1024)+' KB)';
});
drop.addEventListener('dragover',function(e){e.preventDefault();drop.style.borderColor='#58a6ff';});
drop.addEventListener('dragleave',function(){drop.style.borderColor='#30363d';});
drop.addEventListener('drop',function(e){
  e.preventDefault();drop.style.borderColor='#30363d';
  var f=e.dataTransfer.files[0];
  if(f&&f.name.endsWith('.bin')){var dt=new DataTransfer();dt.items.add(f);inp.files=dt.files;
    document.getElementById('fname').textContent=f.name+' ('+Math.round(f.size/1024)+' KB)';}
});
function startUpload(){
  if(!inp.files[0]){alert('Bitte zuerst eine .bin Datei auswaehlen!');return;}
  var fd=new FormData();fd.append('firmware',inp.files[0]);
  var xhr=new XMLHttpRequest();xhr.open('POST','/ota-upload');
  document.getElementById('prog').style.display='block';
  xhr.upload.onprogress=function(e){
    if(e.lengthComputable){var pct=Math.round(e.loaded/e.total*100);
      document.getElementById('bar').style.width=pct+'%';
      document.getElementById('msg').textContent='Hochladen: '+pct+'%';}
  };
  xhr.onload=function(){
    if(xhr.status===200){document.getElementById('bar').style.width='100%';
      document.getElementById('bar').style.background='#3fb950';
      document.getElementById('msg').textContent='Erfolgreich! ESP startet neu...';}
    else{document.getElementById('msg').textContent='Fehler: '+xhr.responseText;
      document.getElementById('bar').style.background='#f85149';}
  };
  xhr.onerror=function(){document.getElementById('msg').textContent='Verbindungsfehler.';};
  xhr.send(fd);
}
</script></body></html>)HTML";

// ── helpers ──────────────────────────────────────────────────────

static String macNoColon() {
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    mac.toUpperCase();
    return mac;
}

static String fmtUptime(unsigned long s) {
    if (s < 60) return String(s) + "s";
    if (s < 3600) return String(s / 60) + "min " + String(s % 60) + "s";
    return String(s / 3600) + "h " + String((s % 3600) / 60) + "min";
}

void WebUi::begin(const String& deviceName, const String& hubHost, int hubPort,
                  unsigned long heartbeatIntervalMs) {
    updateConfig(deviceName, hubHost, hubPort, heartbeatIntervalMs);
    registerRoutes();
    webServer_.begin();
    Serial.println("[WEB] http://" + WiFi.localIP().toString() + "/");
}

void WebUi::updateConfig(const String& deviceName, const String& hubHost, int hubPort,
                         unsigned long heartbeatIntervalMs) {
    deviceName_ = deviceName;
    hubHost_ = hubHost;
    hubPort_ = hubPort;
    heartbeatIntervalMs_ = heartbeatIntervalMs;
}

void WebUi::registerRoutes() {
    webServer_.on("/", HTTP_GET, [this]() { handleRoot(); });
    webServer_.on("/events", HTTP_GET, [this]() { handleEvents(); });
    webServer_.on("/api/status", HTTP_GET, [this]() { handleStatus(); });
    webServer_.on("/api/diag/boot", HTTP_GET, [this]() {
        webServer_.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
        webServer_.send(200, F("application/json"), PanicTail::buildDiagJson());
    });
    webServer_.on("/api/diag/saved", HTTP_GET, [this]() {
        webServer_.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
        webServer_.send(200, F("application/json"), DiagSaved::loadJson());
    });
    webServer_.on("/api/diag/log", HTTP_GET, [this]() {
        webServer_.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
        webServer_.send(200, F("application/json"), DiagLog::buildLogJson());
    });
    webServer_.on("/ota", HTTP_GET, [this]() { handleOtaPage(); });
    webServer_.on("/ota-upload", HTTP_POST,
                  [this]() { handleOtaUploadFinish(); },
                  [this]() { handleOtaUpload(); });
    webServer_.onNotFound([this]() { handleNotFound(); });
}

void WebUi::handleClient() { webServer_.handleClient(); }

void WebUi::pushSseIfDue(unsigned long nowMs) {
    if (nowMs - lastSseSend_ >= 4000UL) {
        lastSseSend_ = nowMs;
        if (sseClient_ && sseClient_.connected()) sseSend(buildStatusJson());
    }
}

String WebUi::buildStatusJson() const {
    updateIoValues();
    String ios = "{";
    for (int i = 0; i < IO_COUNT; i++) {
        if (i > 0) ios += ",";
        ios += "\"" + ioTable[i].key + "\":{\"v\":" + String(ioTable[i].value, 2)
             + ",\"u\":\"" + ioTable[i].unit + "\",\"t\":\"" + ioTable[i].type + "\"}";
    }
    ios += "}";
    const String chip = hw_ ? hw_->chipModel() : String(ESP.getChipModel());
    const uint32_t heap = hw_ ? hw_->freeHeap() : ESP.getFreeHeap();
    const size_t psramFree = hw_ ? hw_->freePsram() : ESP.getFreePsram();
    const size_t psramSize = hw_ ? hw_->psramSize() : ESP.getPsramSize();
    const uint32_t sketch = hw_ ? hw_->freeSketchSpace() : ESP.getFreeSketchSpace();
    String j = "{";
    j += "\"name\":\"" + deviceName_ + "\"";
    j += ",\"chip\":\"" + chip + "\"";
    j += ",\"mac\":\"" + macNoColon() + "\"";
    j += ",\"ip\":\"" + WiFi.localIP().toString() + "\"";
    j += ",\"rssi\":" + String(WiFi.RSSI());
    j += ",\"uptime\":\"" + fmtUptime(millis() / 1000UL) + "\"";
    j += ",\"heap\":" + String(heap);
    j += ",\"psram\":" + String(psramFree);
    j += ",\"psramSize\":" + String(psramSize);
    j += ",\"sketch\":" + String(sketch);
    j += ",\"hub\":\"" + hubHost_ + ":" + String(hubPort_) + "\"";
    j += ",\"interval\":" + String(heartbeatIntervalMs_ / 1000);
    if (hw_) hw_->appendStatusFields(j);
    PanicTail::appendStatusFields(j);
    Checkpoint::appendStatusFields(j);
    DiagLog::appendStatusFields(j);
    j += ",\"ios\":" + ios + "}";
    return j;
}

void WebUi::sseSend(const String& data) {
    if (!sseClient_ || !sseClient_.connected()) return;
    sseClient_.print("data: ");
    sseClient_.print(data);
    sseClient_.print("\n\n");
    sseClient_.flush();
}

void WebUi::handleRoot() {
    String page = F("<!DOCTYPE html><html lang=\"de\"><head>"
                    "<meta charset=\"UTF-8\">"
                    "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
                    "<title>ESP-Hub</title><style>");
    page += FPSTR(PAGE_CSS);
    page += F("</style></head><body>");
    page += FPSTR(PAGE_BODY);
    page += FW_VERSION;
    page += FPSTR(PAGE_SCRIPT);
    page += FW_VERSION;
    page += FPSTR(PAGE_SCRIPT2);
    webServer_.send(200, "text/html", page);
}

void WebUi::handleStatus() {
    webServer_.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    webServer_.send(200, F("application/json"), buildStatusJson());
}

void WebUi::handleEvents() {
    if (sseClient_ && sseClient_.connected()) sseClient_.stop();
    sseClient_ = webServer_.client();
    sseClient_.print(F("HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/event-stream\r\n"
                       "Cache-Control: no-cache\r\n"
                       "Connection: keep-alive\r\n"
                       "Access-Control-Allow-Origin: *\r\n"
                       "\r\n"));
    sseClient_.flush();
    sseSend(buildStatusJson());
    Serial.println("[SSE] Client: " + sseClient_.remoteIP().toString());
}

void WebUi::handleOtaPage() {
    String page = FPSTR(OTA_PAGE);
    page += deviceName_;
    page += FPSTR(OTA_SCRIPT);
    page += FW_VERSION;
    page += FPSTR(OTA_SCRIPT2);
    webServer_.send(200, "text/html", page);
}

void WebUi::handleOtaUpload() {
    HTTPUpload& upload = webServer_.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[WEB-OTA] Start: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
            Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true))
            Serial.printf("\n[WEB-OTA] OK %u Bytes\n", upload.totalSize);
        else
            Update.printError(Serial);
    }
}

void WebUi::handleOtaUploadFinish() {
    if (Update.hasError()) {
        webServer_.send(500, F("text/plain"), F("OTA fehlgeschlagen!"));
        return;
    }
    webServer_.send(200, F("text/plain"), F("OK - Neustart..."));
    PanicTail::requestRestart(PanicTail::RestartWebOta);
}

void WebUi::handleNotFound() {
    webServer_.sendHeader(F("Location"), F("/"), true);
    webServer_.send(302, F("text/plain"), F(""));
}
