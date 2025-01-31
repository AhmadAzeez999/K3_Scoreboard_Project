// Smooth scroll functionality
document.querySelectorAll('.sidebar nav ul li a').forEach(anchor =>
{
    anchor.addEventListener('click', function(e)
    {
        e.preventDefault();
        document.querySelector(this.getAttribute('href')).scrollIntoView(
        {
            behavior: 'smooth'
        });
    });
});

// Search bar functionality
document.getElementById('searchBar').addEventListener('input', function(e)
{
    const query = e.target.value.toLowerCase();
    const sections = document.querySelectorAll('.content section');

    sections.forEach(section =>
    {
        const textContent = section.textContent.toLowerCase();
        if (textContent.includes(query))
        {
            section.style.display = 'block';
        }
        else
        {
            section.style.display = 'none';
        }
    });
});
