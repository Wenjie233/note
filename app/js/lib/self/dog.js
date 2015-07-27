//自制模板引擎
function dogTpl(tpl) {
    var re = /\{\{([^}]+)\}\}/g;

    var implant = function(model) {
        if(model === undefined) {
            return tpl;
        }
        var html = '', i, len;

        if(model instanceof Array) {
            for(i = 0, len = model.length; i < len; i ++) {
                html += tpl.replace(re, function (word, key) {
                    return model[i][key];
                })
            }
        } else {
            html +=  tpl.replace(re, function (word, key) {
                return model[key];
            });
        }

        return html;
    }

    this.render = function (dom, model) {
        dom.innerHTML = implant(model);
    }



    this.implant = implant;
}
