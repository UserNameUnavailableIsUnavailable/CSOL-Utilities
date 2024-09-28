function Copy(object) {
    object.select();
    object.setSelectionRange(0, 99999);
    navigator.clipboard.writeText(object.value).then(
        function () {
            alert("复制成功。");
        },
        function () {
            alert("复制失败。");
        }
    )
}