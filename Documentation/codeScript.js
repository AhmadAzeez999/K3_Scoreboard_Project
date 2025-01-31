document.addEventListener('DOMContentLoaded', () =>
{
    loadFunctionCode('updateAllStrips', './Code/updateAllStrips.txt');
    loadFunctionCode('initializeAllStrips', './Code/initializeAllStrips.txt');
    loadFunctionCode('fillBoard', './Code/fillBoard.txt');
    loadFunctionCode('displayPixel', './Code/displayPixel.txt');
    loadFunctionCode('clearBoard', './Code/clearBoard.txt');
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