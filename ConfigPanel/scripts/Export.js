function LegacyExport(file_name, content) {
    const blob = new Blob([content], { type: "text/plain" })
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = file_name
    a.click();
    URL.revokeObjectURL(url);
}

async function ModernExport(file_name, content) {
    const hFile = await window.showSaveFilePicker({
        suggestedName: file_name,
        types: [{
            description: "Lua source file",
            accept: {
                'text/plain': ['.lua']
            }
        }]
    });
    const ofstream = await hFile.createWritable();
    await ofstream.write(content);
    await ofstream.close();
}

async function Export(file_name, content) {
    try {
        await ModernExport(file_name, content)
    } catch (e) {
        if (e.name === "AbortError") {
            alert("导出操作取消。")
            return // 操作被用户中止，不认为是错误
        }
        alert("尝试显示文件保存选择界面失败，这可能是您的浏览器不支持直接选择文件导出路径。确认后将尝试采用传统方式下载待导出文件，建议在导出后对文件解除锁定。")
        LegacyExport(file_name, content)
    }
}