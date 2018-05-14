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

    function elementInViewport(element) {
        return (element.offsetTop <= (window.scrollY + window.innerHeight + (element.clientHeight * 0.5))) &&
            ((element.offsetTop + element.clientHeight) >= (window.scrollY - (element.clientHeight * 0.5)));
    }

    window.addEventListener('scroll', function () {
        images.forEach(function (img) {
            if (!elementInViewport(img)) {
                img.style.visibility = 'hidden';
            } else {
                img.style.visibility = 'visible';
            }
        });
    });
});
