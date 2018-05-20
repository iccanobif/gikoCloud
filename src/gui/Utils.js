.pragma library

function parseMessage(msg, requestLoginInfo, enterStage)
{
    msg = String(msg);

    if (msg.length == 0) {
        return false;
    }

    var msg = msg.split(" ")
    var stage = msg.length > 1 ? msg[1] : undefined
    msg = msg[0];

    switch(msg) {
    case "/list":
    case "/rula":
    case "#rula":
    case "#list":
        requestLoginInfo();
        return true;
    case "/goto":
    case "#goto":
        if (stage == undefined) {
            return false;
        }

        enterStage(stage)
        return true;
    }

    return false;
}

function escapeHtmlEntities(msg)
{
    msg = String(msg);

    if (msg.length == 0) {
        return msg;
    }

    msg = msg.replace("&", "&amp;");
    msg = msg.replace("<", "&lt;");
    msg = msg.replace(">", "&gt;");

    return msg;
}

function stageNameToId(name)
{
    name = String(name);

    switch(name) {
    case "Shrine":
        return "jinja";
    case "Shrine Entrance":
        return "jinja_st";
    case "Eel Track":
        return "long_st";
    case "Schoolyard":
        return "school_ground";
    case "School Crossing":
        return "school_st";
    case "School Classroom":
        return "school";
    case "School Hallway":
        return "school_rouka";
    case "School Cultural Room":
        return "school_international";
    case "School Computer Lab":
        return "school_pc";
    case "Bar 774":
        return "bar774";
    case "Drunker's Bar":
        return "izakaya774";
    case "Underground Town":
        return "basement";
    case "Bar":
        return "bar";
    case "Bar Street":
        return "bar_st";
    case "Ramen Stand":
        return "yatai";
    case "Admin Street":
        return "admin_st";
    case "Admin's Bar":
        return "admin_bar";
    case "Developer's Lounge":
        return "admin";
    case "Banqueting Hall":
        return "enkai";
    case "Cafe Vista":
        return "cafe_st";
    case "Well A":
        return "idoA";
    case "Well B":
        return "idoB";
    case "Bus Stop":
        return "busstop";
    case "Seashore":
        return "seashore";
    case "Radio Studio":
        return "radio";
    case "Radio Room 1":
        return "radio_room1";
    case "Dressing Room":
        return "radio_gakuya";
    case "Backstage":
        return "radio_backstage";
    case "Beat Giko":
        return "radio_room2";
    case "G-Squid":
        return "radio_room3";
    case "Hilltop":
        return "takadai";
    case "Hilltop Stairway":
        return "kaidan";
    }
    return undefined
}

function stageIdToName(sid)
{
    switch (sid) {
    case "jinja":
        return "Shrine";
    case "jinja_st":
        return "Shrine Entrance";
    case "long_st":
        return "Eel Track";
    case "school_ground":
        return "Schoolyard";
    case "school_st":
        return "School Crossing";
    case "school":
        return "School Classroom";
    case "school_rouka":
        return "School Hallway";
    case "school_international":
        return "School Cultural Room";
    case "school_pc":
        return "School Computer Lab";
    case "bar774":
        return "Bar774";
    case "izakaya774":
        return "Drunker's Bar";
    case "basement":
        return "Underground Town";
    case "bar":
        return "Bar";
    case "bar_st":
        return "Bar Street";
    case "yatai":
        return "Ramen Stand";
    case "admin_st":
        return "Admin Street";
    case "admin_bar":
        return "Admin's Bar";
    case "admin":
        return "Developer's Lounge";
    case "enkai":
        return "Banqueting Hall";
    case "cafe_st":
        return "Cafe Vista";
    case "idoA":
        return "Well A";
    case "idoB":
        return "Well B";
    case "busstop":
        return "Bus Stop";
    case "seashore":
        return "Seashore";
    case "radio":
        return "Radio Studio";
    case "radio_room1":
        return "Radio Room 1";
    case "radio_gakuya":
        return "Dressing Room";
    case "radio_backstage":
        return "Backstage";
    case "radio_room2":
        return "Beat Giko";
    case "radio_room3":
        return "G-Squid";
    case "kaidan":
        return "Hilltop";
    case "takadai":
        return "Hilltop Stairway";
    case "dogrun":
        return "Dogrun";
    case "forest":
        return "Forest";
    case "cafe":
        return "Cafe";
    case "test1":
        return "Test1";
    case "test2":
        return "Test2";
    case "test3":
        return "Test3";
    case "test4":
        return "Test4";
    case "test5":
        return "Test5";
    case "test6":
        return "Test6";
    case "test7":
        return "Test7";
    case "test8":
        return "Test8";
    case "test9":
        return "Test9";
    case "test10":
        return "Test10";
    }
    return undefined;
}
