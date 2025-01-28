document.addEventListener('DOMContentLoaded', () =>
{
    loadFunctionCode('sendBitx8', './Code/sendBitx8.txt');
    loadFunctionCode('sendRowRGB', './Code/sendRowRGB.txt');
    loadFunctionCode('show', './Code/show.txt');
});

// Load code from an external file and insert it into the function content area
function loadFunctionCode(functionId, filePath)
    {
        fetch(filePath)
        .then(response => response.text())
        .then(data =>
        {
            document.getElementById(functionId + '-content').textContent = data;
        })
        .catch(err => console.error('Error loading function code:', err));
}
