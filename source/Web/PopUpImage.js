function ShowPopUpImage(image_path)
{
    const object = document.getElementById("PopUpImage");
    if (!object) return;
    object.src = image_path;
    object.style.display = "block";
}

function HidePopUpImage()
{
    const object = document.getElementById("PopUpImage");
    if (!object) return;
    object.style.display = "none";
}