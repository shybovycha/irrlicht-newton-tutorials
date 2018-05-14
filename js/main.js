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

    function lazyLoadImage(src, elt) {
        return new Promise(function (resolve, reject) {
            var img = new Image();

            img.onload = function () {
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

    setTimeout(function () {
        var images = [].slice.apply(document.querySelectorAll('img'));

        Promise.all(images.map(function (img) {
            return lazyLoadImage(img.getAttribute('data-src'), img);
        })).then(function (images) {
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
    }, 0);
});
