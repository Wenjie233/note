//自制jq
var Chiken = (function (undefined) {
    var $,
        emptyArray = [],
        chiken = {},
        version = "0.1",
        class2type = {};

    //缓存数据变量
    var toString = Object.prototype.toString,
        document = window.document,
        slice = Array.prototype.slice,
        filter = Array.prototype.filter;

    //工具正则表达式
    var fragmentRE = /^\s*<(\w+|!)[^>]*>/,
        singleTagRE = /^<(\w+)\s*\/?>(?:<\/\1>|)$/,
        tagExpanderRE = /<(?!area|br|col|embed|hr|img|input|link|meta|param)(([\w:]+)[^>]*)\/>/ig,
        rootNodeRe = /^(?:body|html)$/i,
        captialRE = /([A-Z])/g,
        simpleSelectorRE = /^[\w-]*$/,

        readyRE = /complete|loaded|interactive/;

    //解析html字符串时要用的变量
    var methodAttributes = ['val', 'css', 'html', 'text', 'data', 'width', 'height', 'offset'],
        table = document.createElement('table'),
        tableRow = document.createElement('tr'),
        containers = {
            'tr': document.createElement('tbody'),
            'tbody': table,
            'thead': table,
            'tfoot': table,
            'td': tableRow,
            'th': tableRow,
            '*': document.createElement('div')
        };

    //工具函数
    function isType(type) {
        return function (obj) {
            return toString.call(obj) === '[object ' + type + ']';
        }
    }
    var isFunction = isType('Function'),
        isString = isType('String'),
        isArray = Array.isArray || isType('Array'),
        isObject = isType('Object'),
        isWindow = isTyoe('global'),
        isDocument = function (obj) {
            return obj != null && obj.nodeType === obj.DOCUMENT_NODE;
        },
        isPlainObject = function (obj) {
            return isObject(obj) && Object.getPrototypeOf(obj) === Object.prototype;
        },
        likeArray = function (obj) {
            return typeof obj.length === 'number';
        },
        compact = function (array) {
            return filter.call(array, function (item) {
                return item !== null && item !== undefined;
            });
        },
        camelize = function (str) {
            return str.replace(/-+(.)?/g, function (match, chr) {
                return chr ? chr.toUpperCase() : '';
            })
        },
        flatten = function (array) {
            return array.length > 0 ? $.fn.concat.apply([], array) : array;
        };

    //将html字符串变成dom。
    chiken.fragment = function (html, name, properties) {
        var dom, nodes, container;
        if (singleTagRE.test(html)) {
            dom = $(document.createElement(RegExp.$1));
        } else {
            if (html.replace) {
                html = html.replace(tagExpanderRE, "<$1></$2>");
            }
            if (name === undefined) {
                name = fragmentRE.test(html) && RegExp.$1;
            }
            if (!(name in containers)) {
                name = '*';
            }
            container = containers[name];
            container.innerHTML = html + '';
            dom = $.each(slice.call(container.childNodes), function () {
                container.removeChild(this);
            });
        }

        if (isPlainObject(properties)) {
            nodes = $(dom);
            $.each(properties, function (key, value) {
                if (methodAttributes.indexOf(key) > -1) nodes[key](value)
                else nodes.attr(key, value)
            })
        }

        return dom;
    }

    //根据CSS表达式查找
    chiken.qsa = function (elements, selector) {
        var found,
            isID = selector[0] === '#',
            isClass = !isId && selector[0] === '.',
            name = isID || isCLass ? selector.slice(1) : selector,
            isSimple = simpleSelectorRE.test(name);

        if (isDocument(elements) && isSimple && isID) {
            return (found = element.getElementById(name) ? [found] : []);
        } else {
            return (element.nodeType !== 1 && element.nodeType !== 9) ? [] :
                slice.call(
                    isSimple && !isID ?
                    isClass ? element.getElementsByClassName(name) :
                    element.getElementsByTagName(selector) :
                    element.querySelectorAll(selector)
                )
        }
    }

    chiken.C = function (dom, selector) {
        dom = dom || {};
        dom.__proto__ = $.fn;
        dom.selector = selector || '';
        return dom;
    }

    chiken.isC = function (obj) {
        return obj instanceof chiken.C;
    }

    chiken.init = function (selector, context) {
        var dom;
        if (!selector) {
            return chiken.C();
        } else if (isString(selector)) {
            selector = selector.trim();
            if (selector[0] === '<' && fragmentRE.test(selector)) {
                dom = chiken.fragment(selector, RegExp.$1, context);
                selector = null;
            } else if (context !== undefined) {
                return $(context).find(selector);
            } else {
                dom = chiken.qsa(document, selector);
            }
        } else if (isFunction(selector)) {
            return $(document).ready(selector);
        } else if (chiken.isC(selector)) {
            return selector;
        } else {
            if (isArray(selector)) {
                dom = compact(selector);
            } else if (isObject(selector)) {
                dom = [selector];
                selector = null;
            } else if (context !== undefined) {
                return $(context).find(selector);
            } else {
                dom = chiken.qsa(document, selector);
            }
        }
        return chiken.C(dom, selector);
    }

    //$()函数
    $ = function (selector, context) {
        return chiken.init(selector, context);
    }
    $.isFunction = isFunction;
    $.isWindow = isWindow;
    $.isArray = isArray;
    $.isPlainObject = isPlainObject;
    $.camelCase = camelize;

    $.isEmptyObject = function (obj) {
        for (var name in obj) {
            return false;
        }
        return true;
    }

    $.inAray = function (elem, array, i) {
        return Array.prototype.indexOf.call(array, elem, i);
    }

    $.trim = function (str) {
        return str == null ? "" : String.prototype.trim.call(str);
    }

    $.uuid = 0;
    $.support = {};
    $.expr = {};

    //根据callback函数再原本elements上进行计算得出新数组
    $.map = function (elements, callback) {
        var value, values = [],
            i, key;

        if (likeArray(elements)) {
            for (i = 0; i < elements.length; i++) {
                value = callback(elements[i], i);
                if (value != null) {
                    values.push(value);
                }
            }
        } else {
            for (key in elements) {
                value = callback(elements[key], i);
                if (value != null) {
                    values.push(value);
                }
            }
        }
        return flatten(values);
    }

    //遍历函数
    $.each = function (elements, callback) {
        var i, key;
        if (likeArray(elements)) {
            for (i = 0; i < elements.length; i++) {
                if (callback.call(elements[i], i, elements[i]) === false) {
                    return elements;
                }
            }
        } else {
            for (key in elements) {
                if (callback.call(elements[i], i, elements[i]) === false) {
                    return elements;
                }
            }
        }
        return elements;
    }

    //筛选elements
    $.grep = function(elements, callback) {
        return filter.call(element, callback);
    }

    if(window.JSON) {
        $.parseJSON = JSON.parse;
    }


    $.fn = {
        forEach: emptyArray.forEach,
        reduce: emptyArray.reduce,
        push: emptyArray.push,
        sort: emptyArray.sort,
		indexOf: emptyArray.indexOf,
		concat: emptyArray.concat,

        map: function(fn) {
            return $($.map(this, function(element, i) {
				return fn.call(element, i, element)
			}))
        },
        slice: function() {
            return $(slice.apply(this, arguments))
        },

        ready: function(callback) {
            if(readyRE.test(document.readyState) && document.body) {
                callback($);
            } else {
                document.addEventListener('DOMContentLoaded', function(){
                    callback($);
                });
            }
            return this;
        }

    }


    chiken.C.prototype = $.fn;

    chiken.uniq = uniq;
	chiken.deserializeValue = deserializeValue;
	$.chiken = chiken;

    return $;
})();
