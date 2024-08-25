async function Export() {
    let lua_snippet = "if (not Setting_lua)\r\n" +
        "then\r\n" +
        "Setting_lua = true\r\n" +
        "Setting = {\r\n";
    let TIME_ZONE = document.getElementById("TIME_ZONE").value;
    if (!IsFieldValid(TIME_ZONE)) { alert(`字段 TIME_ZONE 未配置！`); return; }
    lua_snippet += `\tTIME_ZONE = ${TIME_ZONE},\r\n`;

    let HALL_ROOM_LIST_X = document.getElementById("HALL_ROOM_LIST_X").value;
    if (!IsFieldValid(HALL_ROOM_LIST_X)) { alert(`字段 HALL_ROOM_LIST_X 未配置！`); return; }
    lua_snippet += `\tHALL_ROOM_LIST_X = ${HALL_ROOM_LIST_X},\r\n`;

    let HALL_ROOM_LIST_Y = document.getElementById("HALL_ROOM_LIST_Y").value;
    if (!IsFieldValid(HALL_ROOM_LIST_Y)) { alert(`字段 HALL_ROOM_LIST_Y 未配置！`); return; }
    lua_snippet += `\tHALL_ROOM_LIST_Y = ${HALL_ROOM_LIST_Y},\r\n`;

    let HALL_CREATE_ROOM_X = document.getElementById("HALL_CREATE_ROOM_X").value;
    if (!IsFieldValid(HALL_CREATE_ROOM_X)) { alert(`字段 HALL_CREATE_ROOM_X 未配置！`); return; }
    lua_snippet += `\tHALL_CREATE_ROOM_X = ${HALL_CREATE_ROOM_X},\r\n`;

    let HALL_CREATE_ROOM_Y = document.getElementById("HALL_CREATE_ROOM_Y").value;
    if (!IsFieldValid(HALL_CREATE_ROOM_Y)) { alert(`字段 HALL_CREATE_ROOM_Y 未配置！`); return; }
    lua_snippet += `\tHALL_CREATE_ROOM_Y = ${HALL_CREATE_ROOM_Y},\r\n`;

    let HALL_BACK_X = document.getElementById("HALL_BACK_X").value;
    if (!IsFieldValid(HALL_BACK_X)) { alert(`字段 HALL_BACK_X 未配置！`); return; }
    lua_snippet += `\tHALL_BACK_X = ${HALL_BACK_X},\r\n`;

    let HALL_BACK_Y = document.getElementById("HALL_BACK_Y").value;
    if (!IsFieldValid(HALL_BACK_Y)) { alert(`字段 HALL_BACK_Y 未配置！`); return; }
    lua_snippet += `\tHALL_BACK_Y = ${HALL_BACK_Y},\r\n`;

    let GAME_MODE_X = document.getElementById("GAME_MODE_X").value;
    if (!IsFieldValid(GAME_MODE_X)) { alert(`字段 GAME_MODE_X 未配置！`); return; }
    lua_snippet += `\tGAME_MODE_X = ${GAME_MODE_X},\r\n`;

    let GAME_MODE_Y = document.getElementById("GAME_MODE_Y").value;
    if (!IsFieldValid(GAME_MODE_Y)) { alert(`字段 GAME_MODE_Y 未配置！`); return; }
    lua_snippet += `\tGAME_MODE_Y = ${GAME_MODE_Y},\r\n`;

    let ZOMBIE_SCENARIO_MODE_X = document.getElementById("ZOMBIE_SCENARIO_MODE_X").value;
    if (!IsFieldValid(ZOMBIE_SCENARIO_MODE_X)) { alert(`字段 ZOMBIE_SCENARIO_MODE_X 未配置！`); return; }
    lua_snippet += `\tZOMBIE_SCENARIO_MODE_X = ${ZOMBIE_SCENARIO_MODE_X},\r\n`;

    let ZOMBIE_SCENARIO_MODE_Y = document.getElementById("ZOMBIE_SCENARIO_MODE_Y").value;
    if (!IsFieldValid(ZOMBIE_SCENARIO_MODE_Y)) { alert(`字段 ZOMBIE_SCENARIO_MODE_Y 未配置！`); return; }
    lua_snippet += `\tZOMBIE_SCENARIO_MODE_Y = ${ZOMBIE_SCENARIO_MODE_Y},\r\n`;

    let MAP_CHOOSE_LEFT_SCROLL_X = document.getElementById("MAP_CHOOSE_LEFT_SCROLL_X").value;
    if (!IsFieldValid(MAP_CHOOSE_LEFT_SCROLL_X)) { alert(`字段 MAP_CHOOSE_LEFT_SCROLL_X 未配置！`); return; }
    lua_snippet += `\tMAP_CHOOSE_LEFT_SCROLL_X = ${MAP_CHOOSE_LEFT_SCROLL_X},\r\n`;

    let MAP_CHOOSE_LEFT_SCROLL_Y = document.getElementById("MAP_CHOOSE_LEFT_SCROLL_Y").value;
    if (!IsFieldValid(MAP_CHOOSE_LEFT_SCROLL_Y)) { alert(`字段 MAP_CHOOSE_LEFT_SCROLL_Y 未配置！`); return; }
    lua_snippet += `\tMAP_CHOOSE_LEFT_SCROLL_Y = ${MAP_CHOOSE_LEFT_SCROLL_Y},\r\n`;

    let MAP_CHOOSE_RIGHT_SCROLL_X = document.getElementById("MAP_CHOOSE_RIGHT_SCROLL_X").value;
    if (!IsFieldValid(MAP_CHOOSE_RIGHT_SCROLL_X)) { alert(`字段 MAP_CHOOSE_RIGHT_SCROLL_X 未配置！`); return; }
    lua_snippet += `\tMAP_CHOOSE_RIGHT_SCROLL_X = ${MAP_CHOOSE_RIGHT_SCROLL_X},\r\n`;

    let MAP_CHOOSE_RIGHT_SCROLL_Y = document.getElementById("MAP_CHOOSE_RIGHT_SCROLL_Y").value;
    if (!IsFieldValid(MAP_CHOOSE_RIGHT_SCROLL_Y)) { alert(`字段 MAP_CHOOSE_RIGHT_SCROLL_Y 未配置！`); return; }
    lua_snippet += `\tMAP_CHOOSE_RIGHT_SCROLL_Y = ${MAP_CHOOSE_RIGHT_SCROLL_Y},\r\n`;

    let MAP_TRAP_X = document.getElementById("MAP_TRAP_X").value;
    if (!IsFieldValid(MAP_TRAP_X)) { alert(`字段 MAP_TRAP_X 未配置！`); return; }
    lua_snippet += `\tMAP_TRAP_X = ${MAP_TRAP_X},\r\n`;

    let MAP_TRAP_Y = document.getElementById("MAP_TRAP_Y").value;
    if (!IsFieldValid(MAP_TRAP_Y)) { alert(`字段 MAP_TRAP_Y 未配置！`); return; }
    lua_snippet += `\tMAP_TRAP_Y = ${MAP_TRAP_Y},\r\n`;

    let FINISH_CHOOSE_X = document.getElementById("FINISH_CHOOSE_X").value;
    if (!IsFieldValid(FINISH_CHOOSE_X)) { alert(`字段 FINISH_CHOOSE_X 未配置！`); return; }
    lua_snippet += `\tFINISH_CHOOSE_X = ${FINISH_CHOOSE_X},\r\n`;

    let FINISH_CHOOSE_Y = document.getElementById("FINISH_CHOOSE_Y").value;
    if (!IsFieldValid(FINISH_CHOOSE_Y)) { alert(`字段 FINISH_CHOOSE_Y 未配置！`); return; }
    lua_snippet += `\tFINISH_CHOOSE_Y = ${FINISH_CHOOSE_Y},\r\n`;

    let GAME_DIFFICULTY_X = document.getElementById("GAME_DIFFICULTY_X").value;
    if (!IsFieldValid(GAME_DIFFICULTY_X)) { alert(`字段 GAME_DIFFICULTY_X 未配置！`); return; }
    lua_snippet += `\tGAME_DIFFICULTY_X = ${GAME_DIFFICULTY_X},\r\n`;

    let GAME_DIFFICULTY_Y = document.getElementById("GAME_DIFFICULTY_Y").value;
    if (!IsFieldValid(GAME_DIFFICULTY_Y)) { alert(`字段 GAME_DIFFICULTY_Y 未配置！`); return; }
    lua_snippet += `\tGAME_DIFFICULTY_Y = ${GAME_DIFFICULTY_Y},\r\n`;

    let GAME_DIFFICULTY_OPTION_X = document.getElementById("GAME_DIFFICULTY_OPTION_X").value;
    if (!IsFieldValid(GAME_DIFFICULTY_OPTION_X)) { alert(`字段 GAME_DIFFICULTY_OPTION_X 未配置！`); return; }
    lua_snippet += `\tGAME_DIFFICULTY_OPTION_X = ${GAME_DIFFICULTY_OPTION_X},\r\n`;

    let GAME_DIFFICULTY_OPTION_Y = document.getElementById("GAME_DIFFICULTY_OPTION_Y").value;
    if (!IsFieldValid(GAME_DIFFICULTY_OPTION_Y)) { alert(`字段 GAME_DIFFICULTY_OPTION_Y 未配置！`); return; }
    lua_snippet += `\tGAME_DIFFICULTY_OPTION_Y = ${GAME_DIFFICULTY_OPTION_Y},\r\n`;

    let CREATE_ROOM_X = document.getElementById("CREATE_ROOM_X").value;
    if (!IsFieldValid(CREATE_ROOM_X)) { alert(`字段 CREATE_ROOM_X 未配置！`); return; }
    lua_snippet += `\tCREATE_ROOM_X = ${CREATE_ROOM_X},\r\n`;

    let CREATE_ROOM_Y = document.getElementById("CREATE_ROOM_Y").value;
    if (!IsFieldValid(CREATE_ROOM_Y)) { alert(`字段 CREATE_ROOM_Y 未配置！`); return; }
    lua_snippet += `\tCREATE_ROOM_Y = ${CREATE_ROOM_Y},\r\n`;

    let USE_PASSWORD = document.querySelector('input[name="USE_PASSWORD"]').value == "true" ? true : false;
    lua_snippet += `\tUSE_PASSWORD = ${USE_PASSWORD},\r\n`;

    let ROOM_USE_PASSWORD_X = document.getElementById("ROOM_USE_PASSWORD_X").value;
    if (USE_PASSWORD && !IsFieldValid(ROOM_USE_PASSWORD_X)) { alert(`字段 ROOM_USE_PASSWORD_X 未配置！`); return; }
    if (IsFieldValid(ROOM_USE_PASSWORD_X)) lua_snippet += `\tROOM_USE_PASSWORD_X = ${ROOM_USE_PASSWORD_X},\r\n`;

    let ROOM_USE_PASSWORD_Y = document.getElementById("ROOM_USE_PASSWORD_Y").value;
    if (USE_PASSWORD && !IsFieldValid(ROOM_USE_PASSWORD_Y)) { alert(`字段 ROOM_USE_PASSWORD_Y 未配置！`); return; }
    if (IsFieldValid(ROOM_USE_PASSWORD_Y)) lua_snippet += `\tROOM_USE_PASSWORD_Y = ${ROOM_USE_PASSWORD_Y},\r\n`;

    let ROOM_PASSWORD_BOX_X = document.getElementById("ROOM_PASSWORD_BOX_X").value;
    if (USE_PASSWORD && !IsFieldValid(ROOM_PASSWORD_BOX_X)) { alert(`字段 ROOM_PASSWORD_BOX_X 未配置！`); return; }
    if (IsFieldValid(ROOM_PASSWORD_BOX_X)) lua_snippet += `\tROOM_PASSWORD_BOX_X = ${ROOM_PASSWORD_BOX_X},\r\n`;

    let ROOM_PASSWORD_BOX_Y = document.getElementById("ROOM_PASSWORD_BOX_Y").value;
    if (USE_PASSWORD && !IsFieldValid(ROOM_PASSWORD_BOX_Y)) { alert(`字段 ROOM_PASSWORD_BOX_Y 未配置！`); return; }
    if (IsFieldValid(ROOM_PASSWORD_BOX_Y)) lua_snippet += `\tROOM_PASSWORD_BOX_Y = ${ROOM_PASSWORD_BOX_Y},\r\n`;

    let ROOM_PASSWORD_CONFIRM_X = document.getElementById("ROOM_PASSWORD_CONFIRM_X").value;
    if (USE_PASSWORD && !IsFieldValid(ROOM_PASSWORD_CONFIRM_X)) { alert(`字段 ROOM_PASSWORD_CONFIRM_X 未配置！`); return; }
    if (IsFieldValid(ROOM_PASSWORD_CONFIRM_X)) lua_snippet += `\tROOM_PASSWORD_CONFIRM_X = ${ROOM_PASSWORD_CONFIRM_X},\r\n`;

    let ROOM_PASSWORD_CONFIRM_Y = document.getElementById("ROOM_PASSWORD_CONFIRM_Y").value;
    if (USE_PASSWORD && !IsFieldValid(ROOM_PASSWORD_CONFIRM_Y)) { alert(`字段 ROOM_PASSWORD_CONFIRM_Y 未配置！`); return; }
    if (IsFieldValid(ROOM_PASSWORD_CONFIRM_Y)) lua_snippet += `\tROOM_PASSWORD_CONFIRM_Y = ${ROOM_PASSWORD_CONFIRM_Y},\r\n`;

    let ROOM_USE_CUSTOM_PASSWORD = document.querySelector('input[name="ROOM_USE_CUSTOM_PASSWORD"]:checked').value == "true" ? true : false;
    if (IsFieldValid(ROOM_USE_CUSTOM_PASSWORD)) lua_snippet += `\tROOM_USE_CUSTOM_PASSWORD = ${ROOM_USE_CUSTOM_PASSWORD},\r\n`;

    let ROOM_CUSTOM_PASSWORD = document.getElementById("ROOM_CUSTOM_PASSWORD").value;
    if (USE_PASSWORD && ROOM_USE_CUSTOM_PASSWORD && !IsFieldValid(ROOM_CUSTOM_PASSWORD)) { alert(`字段 ROOM_CUSTOM_PASSWORD 未配置！`); return; }
    if (IsFieldValid(ROOM_CUSTOM_PASSWORD)) {
        ROOM_CUSTOM_PASSWORD = ROOM_CUSTOM_PASSWORD.replaceAll(/[^0-9A-Za-z]/g, "");
        lua_snippet += `\tROOM_CUSTOM_PASSWORD = \"${ROOM_CUSTOM_PASSWORD}\",\r\n`;
    }

    let ROOM_START_GAME_X = document.getElementById("ROOM_START_GAME_X").value;
    if (!IsFieldValid(ROOM_START_GAME_X)) { alert(`字段 ROOM_START_GAME_X 未配置！`); return; }
    lua_snippet += `\tROOM_START_GAME_X = ${ROOM_START_GAME_X},\r\n`;

    let ROOM_START_GAME_Y = document.getElementById("ROOM_START_GAME_Y").value;
    if (!IsFieldValid(ROOM_START_GAME_Y)) { alert(`字段 ROOM_START_GAME_Y 未配置！`); return; }
    lua_snippet += `\tROOM_START_GAME_Y = ${ROOM_START_GAME_Y},\r\n`;

    let TEAM = document.querySelector('input[name="TEAM"]:checked').value;
    lua_snippet += `\tCHOOSE_T_CLASS = ${TEAM == "terrorists"},\r\n`;

    let CHOOSE_T_CLASS_X = document.getElementById("CHOOSE_T_CLASS_X").value;
    if (TEAM == "terrorists" && !IsFieldValid(CHOOSE_T_CLASS_X)) { alert(`字段 CHOOSE_T_CLASS_X 未配置！`); return; }
    if (IsFieldValid(CHOOSE_T_CLASS_X)) lua_snippet += `\tCHOOSE_T_CLASS_X = ${CHOOSE_T_CLASS_X},\r\n`;

    let CHOOSE_T_CLASS_Y = document.getElementById("CHOOSE_T_CLASS_Y").value;
    if (TEAM == "terrorists" && !IsFieldValid(CHOOSE_T_CLASS_Y)) { alert(`字段 CHOOSE_T_CLASS_Y 未配置！`); return; }
    if (IsFieldValid(CHOOSE_T_CLASS_Y)) lua_snippet += `\tCHOOSE_T_CLASS_Y = ${CHOOSE_T_CLASS_Y},\r\n`;

    let CLASS_OPTION = document.getElementById("CLASS_OPTION").value;
    if (!IsFieldValid(CLASS_OPTION)) { alert(`字段 CLASS_OPTION 未配置！`); return; }
    lua_snippet += `\tCLASS_OPTION = ${CLASS_OPTION},\r\n`;

    let USE_SPECIAL_ABILITY = document.querySelector('input[name="USE_SPECIAL_ABILITY"]:checked').value == "true" ? true : false;

    let CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME = document.getElementById("CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME").value;
    if (USE_SPECIAL_ABILITY && !IsFieldValid(CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME)) { alert(`字段 CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME 未配置！`); return; }
    lua_snippet += `\tCLASS_SPECIAL_ABILITY_COOL_DOWN_TIME = ${USE_SPECIAL_ABILITY ? CLASS_SPECIAL_ABILITY_COOL_DOWN_TIME : "math.huge"},\r\n`;

    let ZS_GAME_ESC_MENU_CANCEL_X = document.getElementById("ZS_GAME_ESC_MENU_CANCEL_X").value;
    if (!IsFieldValid(ZS_GAME_ESC_MENU_CANCEL_X)) { alert(`字段 ZS_GAME_ESC_MENU_CANCEL_X 未配置！`); return; }
    lua_snippet += `\tZS_GAME_ESC_MENU_CANCEL_X = ${ZS_GAME_ESC_MENU_CANCEL_X},\r\n`;

    let ZS_GAME_ESC_MENU_CANCEL_Y = document.getElementById("ZS_GAME_ESC_MENU_CANCEL_Y").value;
    if (!IsFieldValid(ZS_GAME_ESC_MENU_CANCEL_Y)) { alert(`字段 ZS_GAME_ESC_MENU_CANCEL_Y 未配置！`); return; }
    lua_snippet += `\tZS_GAME_ESC_MENU_CANCEL_Y = ${ZS_GAME_ESC_MENU_CANCEL_Y},\r\n`;

    let GAME_INSUFFICIENT_FUNDS_CONFIRM_X = document.getElementById("GAME_INSUFFICIENT_FUNDS_CONFIRM_X").value;
    if (!IsFieldValid(GAME_INSUFFICIENT_FUNDS_CONFIRM_X)) { alert(`字段 GAME_INSUFFICIENT_FUNDS_CONFIRM_X 未配置！`); return; }
    lua_snippet += `\tGAME_INSUFFICIENT_FUNDS_CONFIRM_X = ${GAME_INSUFFICIENT_FUNDS_CONFIRM_X},\r\n`;

    let GAME_INSUFFICIENT_FUNDS_CONFIRM_Y = document.getElementById("GAME_INSUFFICIENT_FUNDS_CONFIRM_Y").value;
    if (!IsFieldValid(GAME_INSUFFICIENT_FUNDS_CONFIRM_Y)) { alert(`字段 GAME_INSUFFICIENT_FUNDS_CONFIRM_Y 未配置！`); return; }
    lua_snippet += `\tGAME_INSUFFICIENT_FUNDS_CONFIRM_Y = ${GAME_INSUFFICIENT_FUNDS_CONFIRM_Y},\r\n`;

    let GAME_DEAD_PURCHASE_MENU_REBUY_X = document.getElementById("GAME_DEAD_PURCHASE_MENU_REBUY_X").value;
    if (!IsFieldValid(GAME_DEAD_PURCHASE_MENU_REBUY_X)) { alert(`字段 GAME_DEAD_PURCHASE_MENU_REBUY_X 未配置！`); return; }
    lua_snippet += `\tGAME_DEAD_PURCHASE_MENU_REBUY_X = ${GAME_DEAD_PURCHASE_MENU_REBUY_X},\r\n`;

    let GAME_DEAD_PURCHASE_MENU_REBUY_Y = document.getElementById("GAME_DEAD_PURCHASE_MENU_REBUY_Y").value;
    if (!IsFieldValid(GAME_DEAD_PURCHASE_MENU_REBUY_Y)) { alert(`字段 GAME_DEAD_PURCHASE_MENU_REBUY_Y 未配置！`); return; }
    lua_snippet += `\tGAME_DEAD_PURCHASE_MENU_REBUY_Y = ${GAME_DEAD_PURCHASE_MENU_REBUY_Y},\r\n`;

    let GAME_ROUND_CONFIRM_X = document.getElementById("GAME_ROUND_CONFIRM_X").value;
    if (!IsFieldValid(GAME_ROUND_CONFIRM_X)) { alert(`字段 GAME_ROUND_CONFIRM_X 未配置！`); return; }
    lua_snippet += `\tGAME_ROUND_CONFIRM_X = ${GAME_ROUND_CONFIRM_X},\r\n`;

    let GAME_ROUND_CONFIRM_Y = document.getElementById("GAME_ROUND_CONFIRM_Y").value;
    if (!IsFieldValid(GAME_ROUND_CONFIRM_Y)) { alert(`字段 GAME_ROUND_CONFIRM_Y 未配置！`); return; }
    lua_snippet += `\tGAME_ROUND_CONFIRM_Y = ${GAME_ROUND_CONFIRM_Y},\r\n`;

    let CRAFT_PARTS_AUTO_FILL_X = document.getElementById("CRAFT_PARTS_AUTO_FILL_X").value;
    if (!IsFieldValid(CRAFT_PARTS_AUTO_FILL_X)) { alert(`字段 CRAFT_PARTS_AUTO_FILL_X 未配置！`); return; }
    lua_snippet += `\tCRAFT_PARTS_AUTO_FILL_X = ${CRAFT_PARTS_AUTO_FILL_X},\r\n`;

    let CRAFT_PARTS_AUTO_FILL_Y = document.getElementById("CRAFT_PARTS_AUTO_FILL_Y").value;
    if (!IsFieldValid(CRAFT_PARTS_AUTO_FILL_Y)) { alert(`字段 CRAFT_PARTS_AUTO_FILL_Y 未配置！`); return; }
    lua_snippet += `\tCRAFT_PARTS_AUTO_FILL_Y = ${CRAFT_PARTS_AUTO_FILL_Y},\r\n`;

    let CRAFT_PARTS_COMBINE_X = document.getElementById("CRAFT_PARTS_COMBINE_X").value;
    if (!IsFieldValid(CRAFT_PARTS_COMBINE_X)) { alert(`字段 CRAFT_PARTS_COMBINE_X 未配置！`); return; }
    lua_snippet += `\tCRAFT_PARTS_COMBINE_X = ${CRAFT_PARTS_COMBINE_X},\r\n`;

    let CRAFT_PARTS_COMBINE_Y = document.getElementById("CRAFT_PARTS_COMBINE_Y").value;
    if (!IsFieldValid(CRAFT_PARTS_COMBINE_Y)) { alert(`字段 CRAFT_PARTS_COMBINE_Y 未配置！`); return; }
    lua_snippet += `\tCRAFT_PARTS_COMBINE_Y = ${CRAFT_PARTS_COMBINE_Y},\r\n`;

    let CRAFT_PARTS_CLEAR_X = document.getElementById("CRAFT_PARTS_CLEAR_X").value;
    if (!IsFieldValid(CRAFT_PARTS_CLEAR_X)) { alert(`字段 CRAFT_PARTS_CLEAR_X 未配置！`); return; }
    lua_snippet += `\tCRAFT_PARTS_CLEAR_X = ${CRAFT_PARTS_CLEAR_X},\r\n`;

    let CRAFT_PARTS_CLEAR_Y = document.getElementById("CRAFT_PARTS_CLEAR_Y").value;
    if (!IsFieldValid(CRAFT_PARTS_CLEAR_Y)) { alert(`字段 CRAFT_PARTS_CLEAR_Y 未配置！`); return; }
    lua_snippet += `\tCRAFT_PARTS_CLEAR_Y = ${CRAFT_PARTS_CLEAR_Y},\r\n`;

    let STORE_BUY_OPTION_X = document.getElementById("STORE_BUY_OPTION_X").value;
    if (!IsFieldValid(STORE_BUY_OPTION_X)) { alert(`字段 STORE_BUY_OPTION_X 未配置！`); return; }
    lua_snippet += `\tSTORE_BUY_OPTION_X = ${STORE_BUY_OPTION_X},\r\n`;

    let STORE_BUY_OPTION_Y = document.getElementById("STORE_BUY_OPTION_Y").value;
    if (!IsFieldValid(STORE_BUY_OPTION_Y)) { alert(`字段 STORE_BUY_OPTION_Y 未配置！`); return; }
    lua_snippet += `\tSTORE_BUY_OPTION_Y = ${STORE_BUY_OPTION_Y},\r\n`;

    let STORE_BUY_X = document.getElementById("STORE_BUY_X").value;
    if (!IsFieldValid(STORE_BUY_X)) { alert(`字段 STORE_BUY_X 未配置！`); return; }
    lua_snippet += `\tSTORE_BUY_X = ${STORE_BUY_X},\r\n`;

    let STORE_BUY_Y = document.getElementById("STORE_BUY_Y").value;
    if (!IsFieldValid(STORE_BUY_Y)) { alert(`字段 STORE_BUY_Y 未配置！`); return; }
    lua_snippet += `\tSTORE_BUY_Y = ${STORE_BUY_Y},\r\n`;

    let STORE_BUY_CONFIRM_X = document.getElementById("STORE_BUY_CONFIRM_X").value;
    if (!IsFieldValid(STORE_BUY_CONFIRM_X)) { alert(`字段 STORE_BUY_CONFIRM_X 未配置！`); return; }
    lua_snippet += `\tSTORE_BUY_CONFIRM_X = ${STORE_BUY_CONFIRM_X},\r\n`;

    let STORE_BUY_CONFIRM_Y = document.getElementById("STORE_BUY_CONFIRM_Y").value;
    if (!IsFieldValid(STORE_BUY_CONFIRM_Y)) { alert(`字段 STORE_BUY_CONFIRM_Y 未配置！`); return; }
    lua_snippet += `\tSTORE_BUY_CONFIRM_Y = ${STORE_BUY_CONFIRM_Y},\r\n`;

    lua_snippet +=
        "}\r\n" +
        "end";
    console.log(lua_snippet);
    const options = {
        suggestedName: "Setting",
        types: [{
            description: "Lua source file",
            accept: {
                'text/plain': ['.lua']
            }
        }]
    };
    try {
        const file_handle = await window.showSaveFilePicker(options);
        const ofstream = await file_handle.createWritable();
        await ofstream.write(lua_snippet);
        await ofstream.close();
    } catch (error) {
        console.log('Failed to save Setting.lua.');
    }

}

function IsFieldValid(field) {
    return field != undefined && field != "";
}