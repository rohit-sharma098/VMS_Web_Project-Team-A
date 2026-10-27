const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');
const ROOT = __dirname;
const PORT = 3000;
const exe = process.platform === 'win32' ? 'backend.exe' : './backend';
const mime = {'.html':'text/html; charset=utf-8','.css':'text/css; charset=utf-8','.js':'application/javascript; charset=utf-8','.svg':'image/svg+xml','.png':'image/png','.ico':'image/x-icon'};
function run(args){ const r=spawnSync(path.join(ROOT,exe),args,{cwd:ROOT,encoding:'utf8',windowsHide:true}); if(r.error) throw r.error; if(r.status!==0) throw new Error(r.stderr||'C++ backend failed'); try{return JSON.parse(r.stdout||'{}')}catch(e){throw new Error('Backend returned invalid response: '+r.stdout)} }
function send(res,status,obj){res.writeHead(status,{'Content-Type':'application/json; charset=utf-8','Access-Control-Allow-Origin':'*'});res.end(JSON.stringify(obj));}
function body(req){return new Promise((resolve,reject)=>{let d='';req.on('data',c=>{d+=c;if(d.length>1e6)reject(new Error('Request too large'))});req.on('end',()=>{try{resolve(d?JSON.parse(d):{})}catch(e){reject(new Error('Invalid JSON body'))}});req.on('error',reject)})}
http.createServer(async(req,res)=>{
 try{
  const u=new URL(req.url,'http://localhost');
  if(req.method==='GET'&&u.pathname==='/api/visitors'){let a=run(['list']);const q=(u.searchParams.get('q')||'').toLowerCase(),s=u.searchParams.get('status')||'';a=a.filter(v=>(!q||[v.name,v.host,v.phone,v.purpose,String(v.id)].some(x=>x.toLowerCase().includes(q)))&&(!s||v.status===s));return send(res,200,a)}
  if(req.method==='GET'&&u.pathname==='/api/dashboard'){const a=run(['list']),b=run(['dashboard']);return send(res,200,{total:a.length,pending:a.filter(v=>v.status==='Pending').length,approved:a.filter(v=>v.status==='Approved').length,inside:b.length,visitors:a,recent:a.slice(-6).reverse()})}
  if(req.method==='GET'&&u.pathname==='/api/blacklist')return send(res,200,run(['blacklist']));
  if(req.method==='POST'&&u.pathname==='/api/visitors'){const b=await body(req);for(const k of ['name','phone','email','idProof','purpose','host'])if(!String(b[k]||'').trim())return send(res,400,{ok:false,message:`${k} is required`});return send(res,200,run(['register',b.name,b.phone,b.email,b.idProof,b.purpose,b.host,b.pre?'1':'0']))}
  let m=u.pathname.match(/^\/api\/visitors\/(\d+)\/(approve|reject|checkin|checkout)$/);if(req.method==='POST'&&m){const act=m[2],args=act==='approve'||act==='reject'?['decision',m[1],act==='approve'?'approve':'reject']:[act,m[1]];const result=run(args);return send(res,result.ok?200:409,result)}
  if(req.method==='POST'&&u.pathname==='/api/blacklist'){const b=await body(req);if(!b.name||!b.idProof||!b.reason)return send(res,400,{ok:false,message:'Name, ID proof and reason are required'});return send(res,200,run(['blacklist-add',b.name,b.idProof,b.reason]))}
  m=u.pathname.match(/^\/api\/blacklist\/([^/]+)$/);if(req.method==='DELETE'&&m)return send(res,200,run(['blacklist-remove',decodeURIComponent(m[1])]));
  if(req.method==='GET'&&u.pathname==='/api/export'){run(['export']);res.writeHead(200,{'Content-Type':'text/csv; charset=utf-8','Content-Disposition':'attachment; filename="visitor_report.csv"'});return res.end(fs.readFileSync(path.join(ROOT,'data','visitor_report.csv')))}
  if(u.pathname.startsWith('/api/'))return send(res,404,{message:'API route not found'});
  let file=path.join(ROOT,'frontend',u.pathname==='/'?'index.html':decodeURIComponent(u.pathname));if(!file.startsWith(path.join(ROOT,'frontend'))) {res.writeHead(403);return res.end('Forbidden')};if(!fs.existsSync(file)||fs.statSync(file).isDirectory())file=path.join(ROOT,'frontend','index.html');res.writeHead(200,{'Content-Type':mime[path.extname(file)]||'application/octet-stream'});fs.createReadStream(file).pipe(res);
 }catch(e){send(res,500,{ok:false,message:e.message||'Server error'})}
}).listen(PORT,()=>console.log(`VMS ready at http://localhost:${PORT}`));
