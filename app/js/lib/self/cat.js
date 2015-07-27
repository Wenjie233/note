//自制路由轮子
;(function($){
    var methods = {
        init: function(options) {
            var setting = $.extend({
                'hash': '',
                'onSet': function(){},
                'onRemove': function(){},
            }, options);

            if(!setting.hash) {
                return this;
            }

            if(!$.hashchange) {
                $.hashchange = {
                    router: [],
                    prevHash: ''
                };

                $.hashchange.listener = function(){
                    if(window.location.hash === $.hashchange.prevHash) {
                        return;
                    }

                    var aimRouter;
                    $.each($.hashchange.router, function(ele, i){
                        if(this.ruleRE.test(window.location.hash)) {
                            aimRouter = this;
                            return false;
                        }
                    });
                    if(!aimRouter) {
                        window.location.hash = $.hashchange.prevHash;
                        return;
                    }
                    var i = 1, len = aimRouter.params.length, params = {};
                    for(i = 1; i <= len; i++){
                        if(RegExp['$' + i]) {
                            params[aimRouter.params[i - 1]] = RegExp['$' + i];
                        } else {
                            break;
                        }
                    }

                    if(aimRouter.onRemove) {
                        aimRouter.onRemove.call(aimRouter, params);
                    }

                    if(aimRouter.onSet) {
                        aimRouter.onSet.call(aimRouter, params);
                    }

                    console.log(aimRouter);
                    $.hashchange.prevHash = window.location.hash;
                };

                this.bind('hashchange', $.hashchange.listener);
            }

            function strToRegex(str) {
                var params = [];
                var res = str.replace(/\/\{(\w+)\}/g, function(match, key){
                        params.push(key);
                        return '/(\\w+)';
                    });
                console.log(params);
                return {
                    rule: '^'+ res + '$',
                    params: params
                };
            }

            var mid = strToRegex(setting.hash),
                router = {
                    ruleRE: new RegExp(mid.rule),
                    params: mid.params,
                    onSet: setting.onSet,
                    onRemove: setting.onRemove
                };

            $.hashchange.router.push(router);

            if(router.ruleRE.test(window.location.hash) && window.location.hash !== $.hashchange.prevHash) {
                $.hashchange.listener();
            }

            console.log($.hashchange.router);
            return this;
        }
    }

    $.fn.hashchange = function(options) {
        if(Object.prototype.toString.call(options) === '[object Array]' ) {
            for(i = options.length - 1; i >= 0; i--) {
                methods.init.apply(this, [options[i]]);
            }
            return this;
        }

        return methods.init.apply(this, arguments);
    }

})(jQuery)
