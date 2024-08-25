function Import(event) {
    const file = event.files[0];
    let content = "?";
    if (file) {
        const reader = new FileReader();
        reader.onload = function () {
            content = reader.result;
            do_import(content);
        };
        reader.readAsText(file, "UTF-8");
    }
}

function do_import(content) {
    let expression_pattern = /(\w+)\s*=\s*((?:[\w\.]+)|(?:\"(?:[^\"\\]|\\.)*\")|(?:\'(?:[^\'\\]|\\.)*\'))/;

    const result = content.split(/\r?\n/);
    let table = {};
    result.forEach(
        function resolve(item) {
            let match = item.match(expression_pattern);
            if (match) {
                let key = match[1];
                let value = match[2];
                table[key] = value || "";
            }
        }
    );


    let TIME_ZONE = table["TIME_ZONE"] || "";
    document.getElementById("TIME_ZONE").value = TIME_ZONE;

    let HALL_ROOM_LIST_X = table["HALL_ROOM_LIST_X"] || "";
    document.getElementById("HALL_ROOM_LIST_X").value = HALL_ROOM_LIST_X;

    let HALL_ROOM_LIST_Y = table["HALL_ROOM_LIST_Y"] || "";
    document.getElementById("HALL_ROOM_LIST_Y").value = HALL_ROOM_LIST_Y;

    let HALL_CREATE_ROOM_X = table["HALL_CREATE_ROOM_X"] || "";
    document.getElementById("HALL_CREATE_ROOM_X").value = HALL_CREATE_ROOM_X;

    let HALL_CREATE_ROOM_Y = table["HALL_CREATE_ROOM_Y"] || "";
    document.getElementById("HALL_CREATE_ROOM_Y").value = HALL_CREATE_ROOM_Y;

    let HALL_BACK_X = table["HALL_BACK_X"] || "";
    document.getElementById("HALL_BACK_X").value = HALL_BACK_X;

    let HALL_BACK_Y = table["HALL_BACK_Y"] || "";
    document.getElementById("HALL_BACK_Y").value = HALL_BACK_Y;

    let GAME_MODE_X = table["GAME_MODE_X"] || "";
    document.getElementById("GAME_MODE_X").value = GAME_MODE_X;

    let GAME_MODE_Y = table["GAME_MODE_Y"] || "";
    document.getElementById("GAME_MODE_Y").value = GAME_MODE_Y;

    let ZOMBIE_SCENARIO_MODE_X = table["ZOMBIE_SCENARIO_MODE_X"] || "";
    document.getElementById("ZOMBIE_SCENARIO_MODE_X").value = ZOMBIE_SCENARIO_MODE_X;

    let ZOMBIE_SCENARIO_MODE_Y = table["ZOMBIE_SCENARIO_MODE_Y"] || "";
    document.getElementById("ZOMBIE_SCENARIO_MODE_Y").value = ZOMBIE_SCENARIO_MODE_Y;

    let MAP_CHOOSE_LEFT_SCROLL_X = table["MAP_CHOOSE_LEFT_SCROLL_X"] || "";
    document.getElementById("MAP_CHOOSE_LEFT_SCROLL_X").value = MAP_CHOOSE_LEFT_SCROLL_X;

    let MAP_CHOOSE_LEFT_SCROLL_Y = table["MAP_CHOOSE_LEFT_SCROLL_Y"] || "";
    document.getElementById("MAP_CHOOSE_LEFT_SCROLL_Y").value = MAP_CHOOSE_LEFT_SCROLL_Y;

    let MAP_CHOOSE_RIGHT_SCROLL_X = table["MAP_CHOOSE_RIGHT_SCROLL_X"] || "";
    document.getElementById("MAP_CHOOSE_RIGHT_SCROLL_X").value = MAP_CHOOSE_RIGHT_SCROLL_X;

    let MAP_CHOOSE_RIGHT_SCROLL_Y = table["MAP_CHOOSE_RIGHT_SCROLL_Y"] || "";
    document.getElementById("MAP_CHOOSE_RIGHT_SCROLL_Y").value = MAP_CHOOSE_RIGHT_SCROLL_Y;

    let MAP_TRAP_X = table["MAP_TRAP_X"] || "";
    document.getElementById("MAP_TRAP_X").value = MAP_TRAP_X;

    let MAP_TRAP_Y = table["MAP_TRAP_Y"] || "";
    document.getElementById("MAP_TRAP_Y").value = MAP_TRAP_Y;

    let FINISH_CHOOSE_X = table["FINISH_CHOOSE_X"] || "";
    document.getElementById("FINISH_CHOOSE_X").value = FINISH_CHOOSE_X;

    let FINISH_CHOOSE_Y = table["FINISH_CHOOSE_Y"] || "";
    document.getElementById("FINISH_CHOOSE_Y").value = FINISH_CHOOSE_Y;

    let GAME_DIFFICULTY_X = table["GAME_DIFFICULTY_X"] || "";
    document.getElementById("GAME_DIFFICULTY_X").value = GAME_DIFFICULTY_X;

    let GAME_DIFFICULTY_Y = table["GAME_DIFFICULTY_Y"] || "";
    document.getElementById("GAME_DIFFICULTY_Y").value = GAME_DIFFICULTY_Y;

    let GAME_DIFFICULTY_OPTION_X = table["GAME_DIFFICULTY_OPTION_X"] || "";
    document.getElementById("GAME_DIFFICULTY_OPTION_X").value = GAME_DIFFICULTY_OPTION_X;

    let GAME_DIFFICULTY_OPTION_Y = table["GAME_DIFFICULTY_OPTION_Y"] || "";
    document.getElementById("GAME_DIFFICULTY_OPTION_Y").value = GAME_DIFFICULTY_OPTION_Y;

    let CREATE_ROOM_X = table["CREATE_ROOM_X"] || "";
    document.getElementById("CREATE_ROOM_X").value = CREATE_ROOM_X;

    let CREATE_ROOM_Y = table["CREATE_ROOM_Y"] || "";
    document.getElementById("CREATE_ROOM_Y").value = CREATE_ROOM_Y;

    let ROOM_USE_PASSWORD_X = table["ROOM_USE_PASSWORD_X"] || "";
    document.getElementById("ROOM_USE_PASSWORD_X").value = ROOM_USE_PASSWORD_X;

    let ROOM_USE_PASSWORD_Y = table["ROOM_USE_PASSWORD_Y"] || "";
    document.getElementById("ROOM_USE_PASSWORD_Y").value = ROOM_USE_PASSWORD_Y;

    let ROOM_PASSWORD_BOX_X = table["ROOM_PASSWORD_BOX_X"] || "";
    document.getElementById("ROOM_PASSWORD_BOX_X").value = ROOM_PASSWORD_BOX_X;

    let ROOM_PASSWORD_BOX_Y = table["ROOM_PASSWORD_BOX_Y"] || "";
    document.getElementById("ROOM_PASSWORD_BOX_Y").value = ROOM_PASSWORD_BOX_Y;

    let ROOM_PASSWORD_CONFIRM_X = table["ROOM_PASSWORD_CONFIRM_X"] || "";
    document.getElementById("ROOM_PASSWORD_CONFIRM_X").value = ROOM_PASSWORD_CONFIRM_X;

    let ROOM_PASSWORD_CONFIRM_Y = table["ROOM_PASSWORD_CONFIRM_Y"] || "";
    document.getElementById("ROOM_PASSWORD_CONFIRM_Y").value = ROOM_PASSWORD_CONFIRM_Y;

    let ROOM_USE_CUSTOM_PASSWORD = table["ROOM_USE_CUSTOM_PASSWORD"] || "";

    if (ROOM_USE_CUSTOM_PASSWORD == "true") {
        document.getElementById("CUSTOM_PASSWORD_YES").click();
    }
    else {
        document.getElementById("CUSTOM_PASSWORD_NO").click();
    }

    let ROOM_CUSTOM_PASSWORD = table["ROOM_CUSTOM_PASSWORD"] = (table["ROOM_CUSTOM_PASSWORD"] || "").replaceAll(/[^0-9A-Za-z]/g, "");
    document.getElementById("ROOM_CUSTOM_PASSWORD").value = ROOM_CUSTOM_PASSWORD;

    let ROOM_START_GAME_X = table["ROOM_START_GAME_X"] || "";
    document.getElementById("ROOM_START_GAME_X").value = ROOM_START_GAME_X;

    let ROOM_START_GAME_Y = table["ROOM_START_GAME_Y"] || "";
    document.getElementById("ROOM_START_GAME_Y").value = ROOM_START_GAME_Y;

    let CHOOSE_T_CLASS = table["CHOOSE_T_CLASS"] || "";


    if (CHOOSE_T_CLASS == "true") {
        TEAM = "terrorists";
        document.getElementById("TEAM_TR").click();
    }
    else {
        TEAM = "counter-terrorists";
        document.getElementById("TEAM_CT").click();
    }

    let CHOOSE_T_CLASS_X = table["CHOOSE_T_CLASS_X"] || "";
    document.getElementById("CHOOSE_T_CLASS_X").value = CHOOSE_T_CLASS_X;

    let CHOOSE_T_CLASS_Y = table["CHOOSE_T_CLASS_Y"] || "";
    document.getElementById("CHOOSE_T_CLASS_Y").value = CHOOSE_T_CLASS_Y;

    let CLASS_OPTION = table["CLASS_OPTION"] || "";
    document.getElementById("CLASS_OPTION").value = CLASS_OPTION;

    let CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME = table["CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME"] || "";
    document.getElementById("CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME").value = CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME;


    if (CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME == "math.huge") {
        USE_SPECIAL_ABILITY = false;
        document.getElementById("USE_SPECIAL_ABILITY_NO").click();
    }
    else {
        USE_SPECIAL_ABILITY = true;
        document.getElementById("USE_SPECIAL_ABILITY_YES").click();
    }

    let ZS_GAME_ESC_MENU_CANCEL_X = table["ZS_GAME_ESC_MENU_CANCEL_X"] || "";
    document.getElementById("ZS_GAME_ESC_MENU_CANCEL_X").value = ZS_GAME_ESC_MENU_CANCEL_X;

    let ZS_GAME_ESC_MENU_CANCEL_Y = table["ZS_GAME_ESC_MENU_CANCEL_Y"] || "";
    document.getElementById("ZS_GAME_ESC_MENU_CANCEL_Y").value = ZS_GAME_ESC_MENU_CANCEL_Y;

    let GAME_INSUFFICIENT_FUNDS_CONFIRM_X = table["GAME_INSUFFICIENT_FUNDS_CONFIRM_X"] || "";
    document.getElementById("GAME_INSUFFICIENT_FUNDS_CONFIRM_X").value = GAME_INSUFFICIENT_FUNDS_CONFIRM_X;

    let GAME_INSUFFICIENT_FUNDS_CONFIRM_Y = table["GAME_INSUFFICIENT_FUNDS_CONFIRM_Y"] || "";
    document.getElementById("GAME_INSUFFICIENT_FUNDS_CONFIRM_Y").value = GAME_INSUFFICIENT_FUNDS_CONFIRM_Y;

    let GAME_DEAD_PURCHASE_MENU_REBUY_X = table["GAME_DEAD_PURCHASE_MENU_REBUY_X"] || "";
    document.getElementById("GAME_DEAD_PURCHASE_MENU_REBUY_X").value = GAME_DEAD_PURCHASE_MENU_REBUY_X;

    let GAME_DEAD_PURCHASE_MENU_REBUY_Y = table["GAME_DEAD_PURCHASE_MENU_REBUY_Y"] || "";
    document.getElementById("GAME_DEAD_PURCHASE_MENU_REBUY_Y").value = GAME_DEAD_PURCHASE_MENU_REBUY_Y;

    let GAME_ROUND_CONFIRM_X = table["GAME_ROUND_CONFIRM_X"] || "";
    document.getElementById("GAME_ROUND_CONFIRM_X").value = GAME_ROUND_CONFIRM_X;

    let GAME_ROUND_CONFIRM_Y = table["GAME_ROUND_CONFIRM_Y"] || "";
    document.getElementById("GAME_ROUND_CONFIRM_Y").value = GAME_ROUND_CONFIRM_Y;

    let CRAFT_PARTS_AUTO_FILL_X = table["CRAFT_PARTS_AUTO_FILL_X"] || "";
    document.getElementById("CRAFT_PARTS_AUTO_FILL_X").value = CRAFT_PARTS_AUTO_FILL_X;

    let CRAFT_PARTS_AUTO_FILL_Y = table["CRAFT_PARTS_AUTO_FILL_Y"] || "";
    document.getElementById("CRAFT_PARTS_AUTO_FILL_Y").value = CRAFT_PARTS_AUTO_FILL_Y;

    let CRAFT_PARTS_COMBINE_X = table["CRAFT_PARTS_COMBINE_X"] || "";
    document.getElementById("CRAFT_PARTS_COMBINE_X").value = CRAFT_PARTS_COMBINE_X;

    let CRAFT_PARTS_COMBINE_Y = table["CRAFT_PARTS_COMBINE_Y"] || "";
    document.getElementById("CRAFT_PARTS_COMBINE_Y").value = CRAFT_PARTS_COMBINE_Y;

    let CRAFT_PARTS_CLEAR_X = table["CRAFT_PARTS_CLEAR_X"] || "";
    document.getElementById("CRAFT_PARTS_CLEAR_X").value = CRAFT_PARTS_CLEAR_X;

    let CRAFT_PARTS_CLEAR_Y = table["CRAFT_PARTS_CLEAR_Y"] || "";
    document.getElementById("CRAFT_PARTS_CLEAR_Y").value = CRAFT_PARTS_CLEAR_Y;

    let STORE_BUY_OPTION_X = table["STORE_BUY_OPTION_X"] || "";
    document.getElementById("STORE_BUY_OPTION_X").value = STORE_BUY_OPTION_X;

    let STORE_BUY_OPTION_Y = table["STORE_BUY_OPTION_Y"] || "";
    document.getElementById("STORE_BUY_OPTION_Y").value = STORE_BUY_OPTION_Y;

    let STORE_BUY_X = table["STORE_BUY_X"] || "";
    document.getElementById("STORE_BUY_X").value = STORE_BUY_X;

    let STORE_BUY_Y = table["STORE_BUY_Y"] || "";
    document.getElementById("STORE_BUY_Y").value = STORE_BUY_Y;

    let STORE_BUY_CONFIRM_X = table["STORE_BUY_CONFIRM_X"] || "";
    document.getElementById("STORE_BUY_CONFIRM_X").value = STORE_BUY_CONFIRM_X;

    let STORE_BUY_CONFIRM_Y = table["STORE_BUY_CONFIRM_Y"] || "";
    document.getElementById("STORE_BUY_CONFIRM_Y").value = STORE_BUY_CONFIRM_Y;

    console.log(table);
}