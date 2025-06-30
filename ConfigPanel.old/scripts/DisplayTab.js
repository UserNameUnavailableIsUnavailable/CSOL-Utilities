function DisplayTab(tab_to_activate, file_name) {
    var tabs_to_deactivate = document.querySelectorAll(".ActiveTab")
    if (tabs_to_deactivate) {
        tabs_to_deactivate.forEach(element => {
            element.classList.remove("ActiveTab")
            element.classList.add("InactiveTab")
        });
    }
    tab_to_activate.classList.remove("InactiveTab")
    tab_to_activate.classList.add("ActiveTab")
    document.getElementById("ConfigPanel").innerHTML
        = `<object data='${file_name}' width=100% height=100%"></object>`
}
