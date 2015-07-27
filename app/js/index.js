$(function(){

    // var articles =[{
    //     "tag": "设计",
    //     "tagID": 1,
    //     "ID":2,
    //     "title": "用户关注",
    //     "content": "<p>dsdsdsdsd</p>"
    // },{
    //     "tag": "设计",
    //     "tagID":1,
    //     "ID": 3,
    //     "title": "你妹美美",
    //     "content": "<p>dsdsdsdsd</p>"
    // },{
    //     "tag": "设计",
    //     "tagID":1,
    //     "ID": 4,
    //     "title": "四大四大",
    //     "content": "阿斯达施舍的"
    // },{
    //     "tag": "设计",
    //     "tagID":1,
    //     "ID": 5,
    //     "title": "猪头",
    //     "content": "非常"
    // }];
    // var tags = [{
    //     "id": "1",
    //     "tag": "设计"
    // },{
    //     "id": "2",
    //     "tag": "html5"
    // },{
    //     "id": "3",
    //     "tag": "angularjs"
    // },{
    //     "id": "4",
    //     "tag": "css3"
    // }]
    // //
    //  Model.set('articles', articles);
    //  Model.set('tags', tags);
    // 
    // Model.remove('articles');
    // Model.remove('tags');

    if(!Model.getAll()['articles'] && !Model.getAll()['tags']) {
        var tags = [{
            "id": "1",
            "tag": "初始标签"
        }]
        var articles = [{
            "tag": "初始标签",
            "tagID": 1,
            "ID":1,
            "title": "初始文章",
            "content": "<h1>h1</h1> <pre>你是猪吗</pre> <p>ppppp</p>"
        }]
        Model.set('articles', articles);
        Model.set('tags', tags);
    }

    function indexPage($container){
        this.$addNoteWrap = $('.add-note-wrap');
        this.$notebookWrap = $('.notebook-wrap');
        this.$notesWrap = $('.notes-wrap');
        this.$contentWrap = $('.content-wrap');
        this.init();
    }

    indexPage.prototype.init = function(){
        this.initNav()
            .renderNav()
            .initNotes()
            .initWrite()
            .renderWrite()
            // .renderNotes(1)
            // .renderArticle(4);
    }

    //初始化写作区，绑定事件等
    indexPage.prototype.initWrite = function(){
        var that = this;

        var $article = $('.article-wrap>article'),
            $titleIpt = $('.add-title-ipt'),
            $contentTitle = $('.content-title'),
            $writeTextarea = $('.write-textarea');

        // that.$contentWrap.on('change');
        $writeTextarea.on('change', function() {
            $('.article-wrap>article').html(markdown.toHTML($writeTextarea.val()));
        });

        $titleIpt.on('change', function(){
            $('.content-title').html($titleIpt.val());
        });

        that.$addNoteWrap.on('click', '.exit', function(){
            that.hideWrite();
        }).on('click', '.save', function(){
            var title = $titleIpt.val(),
                content = markdown.toHTML($writeTextarea.val()),
                tagID = $('.tag-select option:selected').val(),
                tag = $('.tag-select option:selected').text();

            if(title && content && tagID && tag) {
                Request.addArticle({
                    title: title,
                    content: content,
                    tagID: tagID,
                    tag: tag
                }, function(id){
                    alert('添加成功');
                    that.hideWrite()
                        .clearWrite();
                    go({
                        tagID: tagID,
                        articleID: id
                    })
                })
            } else {
                alert('内容还没填写完成')
            }
        })

        return that;
    }

    //清除写作栏文子
    indexPage.prototype.clearWrite = function(){
        $('.write-textarea').val('');
        $('.add-title-ipt').val('');

        return this;
    }

    //渲染写作栏标签
    indexPage.prototype.renderWrite = function(){
        var tpl = new dogTpl('<option value="{{id}}">{{tag}}</option>');
        var $tagSelect = $('.tag-select');
        $tagSelect.html(tpl.implant(Request.getTags()));
    }

    //初始化框架
    indexPage.prototype.initFrame = function($container) {
        var tpl = new dogTpl('<section class="notebook-wrap">'
                            + '<div class="inner"></div>'
                            + '</section>'
                            + '<section class="notes-wrap">'
                            + '<div class="inner"></div>'
                            + '</section>'
                            + '<section class="content-wrap"></section>');
        $container.html(tpl.implant());
        return this;
    }

    //初始化导航栏
    indexPage.prototype.initNav = function(){
        var that = this;
        var tpl = new dogTpl('<header><h1>笔记本</h1></header>'
                            + '<ul class="tags"></ul>'
                            + '<div class="add-notebook-wrap"><span>＋</span>'
                            + '<section class="add-tag-modal-wrap">'
                            + '<div class="ipt-wrap">'
                            + '<input type="text" class="tag-ipt" name="name" value="" placeholder="请输入标签">'
                            + '</div>'
                            + '<div class="btn-group-wrap">'
                            + '<button class="btn confirm-tag-btn">确认</button>'
                            + '<button class="btn close-tag-btn">关闭</button>'
                            + '</div>'
                            + '</section></div>');
        var $wrap = $('.notebook-wrap>.inner');
        $wrap.html(tpl.implant());

        var $tagsWrap = $('.notebook-wrap .tags'),
            $modal = $('.add-tag-modal-wrap'),
            $tagIpt = $('.tag-ipt');

        $wrap.on('click', '.tags>li', function() {
            $(this).addClass('selected').siblings().removeClass('selected');
            go({tagID: $(this).data('id')})
        }).on('click', '.add-notebook-wrap>span, .close-tag-btn', function() {
            $modal.toggleClass('pop');
        }).on('click', '.confirm-tag-btn', function(){
            if($tagIpt.val()) {
                Request.addTag($tagIpt.val(), function(tagID){
                    $tagIpt.val('');
                    $modal.toggleClass('pop');
                    that.renderNav()
                        .renderNotes(tagID);
                    go({
                        tagID: tagID
                    })
                })

            } else {
                alert('标签不能为空')
            }

        });
        return this;
    }


    indexPage.prototype.initNotes = function() {
        var that = this;
        var tpl = new dogTpl('<header><h1>笔记<div class="add-note">新建笔记</div></h1></header>'
                            +'<ul class="notes-mini-wrap"></ul>');
        var $wrap = $('.notes-wrap>.inner');

        $wrap.html(tpl.implant()).on('click', '.notes-mini-wrap>li', function(){
            $(this).addClass('selected').siblings().removeClass('selected');
            go({articleID: $(this).data('id')})
        }).on('click', '.add-note', function(){
            that.showWrite();
        });

        return this;
    }

    indexPage.prototype.renderNav = function(){
        var tpl = new dogTpl('<li data-id="{{id}}">{{tag}}</li>'),
            $tagsWrap = $('.notebook-wrap .tags');

        Request.getTags(function(tags){
            $tagsWrap.html(tpl.implant(tags))
        })
        return this;

    }

    //渲染文章缩略
    indexPage.prototype.renderNotes = function(tagID, ID) {
        var tpl = new dogTpl('<li data-id="{{ID}}"><strong>{{title}}</strong><p>{{content}}</p></li>'),
            $notesMiniWrap = $('.notes-mini-wrap');

        Request.getArticlesByTagID(tagID, function(articles){
            $.each(articles, function() {
                this.content = safeFont(this.content);
            });
            $notesMiniWrap.html(tpl.implant(articles))
        });

        //在nav里显示选定标示
        $('.notebook-wrap .tags>li').each(function(element, i) {
            $this = $(this);
            if($this.data('id') == tagID) {
                $this.addClass('selected');
            } else {
                $this.removeClass('selected')
            }
        })
        //在左边缩略栏显示选定
        $('.notes-mini-wrap>li').each(function(element, i) {
            $this = $(this);
            if($this.data('id') == ID) {
                $this.addClass('selected');
            } else {
                $this.removeClass('selected')
            }
        })

        return this;
    }
    //渲染文章
    indexPage.prototype.renderArticle = function(ID) {
        var that = this;
        var tpl = new dogTpl('<header><h1><a class="close">×</a>'
                            + '<span class="content-title">{{title}}</span>'
                            + '</h1></header>'
                            + '<div class="article-wrap">'
                            + '<article  class="markdown">{{content}}</article></div>');
        var $contentWrap = $('.content-wrap');
        Request.getArticleByID(ID, function(article){

            if(!article){
                return that;
            }
            //在左边缩略栏显示选定
            $('.notes-mini-wrap>li').each(function(element, i) {
                $this = $(this);
                if($this.data('id') == ID) {
                    $this.addClass('selected');
                } else {
                    $this.removeClass('selected')
                }
            })

            $contentWrap.html(tpl.implant(article));
            $contentWrap.removeClass('hide');
        });


        return this;
    }
    //开关写作区
    indexPage.prototype.toggleWrite = function(){
        this.$addNoteWrap.toggleClass('hide');
        this.$notebookWrap.toggleClass('hide');
        this.$notesWrap.toggleClass('hide');
        return this;
    }

    //显示写作区
    indexPage.prototype.showWrite = function(){
        this.renderWrite();
        this.$addNoteWrap.removeClass('hide');
        this.$notebookWrap.addClass('hide');
        this.$notesWrap.addClass('hide');
        this.$contentWrap.removeClass('hide');
        return this;
    }
    //隐藏写作区
    indexPage.prototype.hideWrite = function(){
        this.$addNoteWrap.addClass('hide');
        this.$notebookWrap.removeClass('hide');
        this.$notesWrap.removeClass('hide');
        return this;
    }


    //数据操作方法封装
    var Request = {
        tags: Model.get('tags'),
        articles: Model.get('articles')
    };

    Request.getTags = function(callback) {
        var data = Model.get('tags', []);
        if(callback) {
            return callback(data);
        } else {
            return data;
        }
    }

    Request.addTag = function(tag, callback) {
        Request.getTags(function(data){
            var id = (data[data.length - 1] && ( parseInt(data[data.length - 1].id) + 1 ) )|| 1;
            data.push({
                tag:tag,
                id: id
            });
            Model.set('tags', data);
            if(callback) {
                callback(id);
            }
        });
    }

    Request.getArticlesByTagID = function(tagID, callback) {
        var all = Model.get('articles', []),
            res = [], i, len;
        for(i = 0, len = all.length; i < len; i++) {
            if(all[i].tagID == tagID) {
                res.push(all[i]);
            }
        }
        if(callback) {
            return callback(res);
        } else {
            return res;
        }
    }

    Request.getArticleByID = function(ID, callback) {
        var all = Model.get('articles', []),
            res = [], i, len;
        for(i = 0, len = all.length; i < len; i++) {
            if(all[i].ID == ID) {
                if(callback) {
                    return callback(all[i]);
                } else {
                    return all[i];
                }
            }
        }
        return callback ? callback(null) : null;
    }

    Request.addArticle = function(article, callback) {
        var all = Model.get('articles', []);
        var ID = (all[all.length - 1] && ( parseInt(all[all.length - 1].ID) + 1 ) )|| 1
        article.ID = ID;
        all.push(article);
        Model.set('articles', all);
        if(callback) {
            return callback(article.ID);
        }
    }

    var page = new indexPage($('.container'));
    var lastParams;
    if( !/^#\/\w+\/\w+(\/)?$/.test(location.hash) ) {
        location.hash = '#/1/1';
    }
    $(window).hashchange({
        hash: '#/{tagID}/{articleID}',
        onSet: function(params){

            page.renderNotes(params.tagID, params.articleID)
                .renderArticle(params.articleID);

            // page.hideWrite();
            // if(lastParams.tagID != params.tagID) {
            //     page.renderNotes(params.tagID, params.articleID)
            // // }
            //
            // // if(lastParams.articleID != params.articleID) {
            //     page.renderArticle(params.articleID)
            // // }
            // lastParams = params;

        },
        onRemove: function(){

        }
    });

    //放function声明函数

    //跳转页面
    function go(params) {
        if(params.tagID && params.articleID) {
            location.hash = '#/' + params.tagID + '/' + params.articleID;
        } else if(params.tagID) {
            location.hash = location.hash.replace(/(^#\/)\w+(\/\w+\/?$)/, '$1'+ params.tagID +'$2')
        } else if (params.articleID) {
            location.hash = location.hash.replace(/(^#\/\w+\/)\w+(\/?$)/, '$1'+ params.articleID +'$2')
        }
    }

    //去掉str里的标签
    function safeFont(str)  {
        return str.replace(/<[^>]*?>/gi, '').replace(/\s|\n/g, '').substr(0, 70);
    }
})
