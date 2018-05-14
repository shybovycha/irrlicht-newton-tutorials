window.addEventListener('DOMContentLoaded', function () {
    var navToggle = document.querySelector('.nav-toggle');
    var nav = document.querySelector('.nav');

    navToggle.addEventListener('click', function (e) {
        e.preventDefault();

        if (nav.classList.contains('open')) {
            nav.classList.remove('open');
            navToggle.innerHTML = '&equiv;';
        } else {
            nav.classList.add('open');
            navToggle.innerHTML = '&times;';
        }
    });

    var images = [].slice.apply(document.querySelectorAll('img'));

    setTimeout(function () {
        images = images.map(function (elt) {
            var img = new Image();

            img.onload = function () {
                elt.parentNode.replaceChild(img, elt);
            };

            img.src = elt.getAttribute('data-src');
        });
    }, 0);

    function elementInViewport(element) {
        return (element.offsetTop <= (window.scrollY + window.innerHeight + (element.clientHeight * 0.5))) &&
            ((element.offsetTop + element.clientHeight) >= (window.scrollY - (element.clientHeight * 0.5)));
    }

    window.addEventListener('scroll', function () {
        // var images = [].slice.apply(document.querySelectorAll('img'));

        images.forEach(function (img) {
            if (!elementInViewport(img)) {
                img.style.visibility = 'hidden';
            } else {
                img.style.visibility = 'visible';
            }
        });
    });
});
