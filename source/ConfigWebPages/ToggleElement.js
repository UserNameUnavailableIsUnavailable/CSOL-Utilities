function ToggleElement(id, flag) {
    const element = document.getElementById(id);
    if (flag) {
        element.style.display = "";
    }
    else {
        element.style.display = "none";
    }
}
