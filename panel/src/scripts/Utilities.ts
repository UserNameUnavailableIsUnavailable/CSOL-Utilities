export async function SaveAs(file_name: string, content: string) {
    // 传统下载方案
    const __impl_legacy_save_as = async (file_name: string, content: string) => {
        const blob = new Blob([content], { type: "text/plain" });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = file_name;
        a.click();
        URL.revokeObjectURL(url);
    };
    // 新版 Edge 和 Chrome 解决方案
    const __impl_modern_save_as = async (file_name: string, content: string) => {
        // @ts-ignore
        const file_picker = await window.showSaveFilePicker({
            suggestedName: file_name,
            types: [{
                description: "Lua source file",
                accept: { "text/plain": [".lua"] }
            }]
        });
        const stream = await file_picker.createWritable();
        await stream.write(content);
        await stream.close();
    };
    try {
        // @ts-ignore
        if (window.showSaveFilePicker) {
            __impl_modern_save_as(file_name, content);
        } else {
            alert("The showSaveFilePicker() method is not supported in your browser, falling back to legacy download method. We recommend using Edge or Chrome for a better experience.");
            __impl_legacy_save_as(file_name, content);
        }
    } catch (e) {
        console.error("Error saving file:", e);
    }
}

export function AddSurroundingQuotes(s: string) {
    if (
        s.startsWith('\'') && s.endsWith('\'') ||
        s.startsWith('\"') && s.endsWith('\"')
    ) {
        return s;
    }
    return `"${s}"`;
}

export function RemoveSurroundingQuotes(s: string) {
    if (
        s.startsWith('\'') && s.endsWith('\'') ||
        s.startsWith('\"') && s.endsWith('\"')
    ) {
        return s.slice(1, -1);        
    }
    return s;
}
