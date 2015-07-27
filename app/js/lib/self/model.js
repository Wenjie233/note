'use strict';

//封装操作localStorage方法
(function(){
    var model = {},
        storage = window.localStorage;

    model.serialize = function(value) {
        return JSON.stringify(value);
    }

    model.deserialize = function(value) {
        if(typeof value !== 'string') {
            return undefined;
        }
        try {
            return JSON.parse(value);
        } catch(e) {
            return value || undefined;
        }
    }

    model.get = function(key, defaultVal) {
        var val = model.deserialize(storage.getItem(key));
        return (val === undefined ? defaultVal : val);
    }

    model.set  = function(key, val) {
        if(val === undefined) {
            return  model.remove(key);
        }
        storage.setItem(key, model.serialize(val))
    }

    model.remove = function(key) {
        storage.removeItem(key);
    }

    model.clear = function() {
        storage.clear();
    }

    model.getAll = function() {
        var res = {};
        model.forEach(function(key, val) {
            res[key] = val;
        });
        return res;
    }

    model.forEach = function(callback){
        var i, len, key;
        for(i = 0, len = storage.length; i < len; i++) {
            var key = storage.key(i);
            callback(key, model.get(key));
        }
    }

    //暴露接口
    window.Model = model;
})()
