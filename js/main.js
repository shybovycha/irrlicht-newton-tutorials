function lazyLoadImage(src, elt) {
    return new Promise(function (resolve, reject) {
        const img = new Image();

        img.onload = () => {
            if (elt) {
                elt.parentNode.replaceChild(img, elt);
            }

            resolve(img);
        };

        img.src = src;
    });
}

function elementInViewport(element) {
    return (element.offsetTop <= (window.scrollY + window.innerHeight + (element.clientHeight * 0.5))) &&
        ((element.offsetTop + element.clientHeight) >= (window.scrollY - (element.clientHeight * 0.5)));
}

function handleImageViewportVisibility(img) {
    if (!elementInViewport(img)) {
        img.style.visibility = 'hidden';
    } else {
        img.style.visibility = 'visible';
    }
}

window.addEventListener('DOMContentLoaded', function () {
    const navToggle = document.querySelector('.nav-toggle');
    const nav = document.querySelector('.nav');

    navToggle.addEventListener('click', (e) => {
        e.preventDefault();

        if (nav.classList.contains('open')) {
            nav.classList.remove('open');
            navToggle.innerHTML = '&equiv;';
        } else {
            nav.classList.add('open');
            navToggle.innerHTML = '&times;';
        }
    });

    // defer lazy image loading to after the page is loaded
    setTimeout(function () {
        const images = Array.from(document.querySelectorAll('img'));

        // we will use the lazy loaded images to process the scroll event
        // thus we can't rely on <img> but rather on Image objects we replace <img>'s with
        Promise.all(images.map(img => lazyLoadImage(img.getAttribute('data-src'), img)))
            .then(function (images) {
                window.addEventListener('scroll', function () {
                    images.forEach(handleImageViewportVisibility);
                });
            });
    }, 0);
});
