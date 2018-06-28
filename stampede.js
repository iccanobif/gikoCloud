let child_process = require("child_process");

processes = [];
function addProcess(proxy)
{
    console.log(proxy);
    let backendProcess = child_process.spawn("src/cli/cli",
        ("-u YHWH -s _for -r yatai -c 1010 -p " + proxy).split(" "));
    backendProcess.stdout.on("data", (data) =>
    {
        console.log(String(data).trim());
    });
    backendProcess.stderr.on("data", (data) =>
    {
        console.log(String(data).trim());
    });
    backendProcess.on("exit", (code, signal) =>
    {
        console.log("Backend process died...");
    });
    processes.push(backendProcess);
}

proxyList = [
    "130.211.109.162:1080",
    "188.120.250.67:5411",
    "95.110.194.245:2090",
    "188.40.10.39:10000",
    "14.123.180.139:1080",
    "121.61.24.35:1080",
    "117.24.37.105:1080"
];

function fanneUno(pl)
{
    if (pl.length == 0)
    {
        let i = 0;
        setInterval(() =>
        {
            let dir;
            let n = Math.random();
            if (n <= 0.25)
                dir = "up";
            else if (n <= 0.5)
                dir = "down";
            else if (n <= 0.75)
                dir = "right";
            else
                dir = "left";
            console.log("move " + dir);
            processes[i].stdin.write("move " + dir + "\n");
            i++;
            if (i >= processes.length)
                i = 0;
        }, 1000);
    }
    else
    {
        addProcess(pl.shift());
        setTimeout(() => { fanneUno(pl) }, 5000);
    }
}

fanneUno(proxyList);