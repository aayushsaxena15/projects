# -*- coding: utf-8 -*-
# this file is released under public domain and you can use without limitations

#########################################################################
## This is a samples controller
## - index is the default action of any application
## - user is required for authentication and authorization
## - download is for downloading files uploaded in the db (does streaming)
## - call exposes all registered services (none by default)
#########################################################################


def index():
    """
    example action using the internationalization operator T and flash
    rendered by views/default/index.html or views/generic.html

    if you need a simple wiki simple replace the two lines below with:
    return auth.wiki()
    """
    
    response.flash = T("Welcome to worldbuzz")
    f=db().select(db.article.category)
    fin=[]
    for i in range(len(f)):
	    u=f[i].category
	    if u not in fin:
		    fin.append(u)
    return dict(message=T('Hello World'),fin=fin,f=f)
@auth.requires_login()
def delete():
	r=auth.user.id
	if auth.user.id>1:
		response.flash=T("Only admin has this privilege")
	s=db(db.auth_user.id>0).select()
	d=[]
	form=SQLFORM.factory(db.Field('deleteid','integer'))
	for i in range(len(s)):
		if s[i].id>1:
			d.append(s[i])
	else:
		if form.process().accepted:
			temp=db(db.auth_user.id==form.vars.deleteid).select()
			a=temp[0].first_name+' '+temp[0].last_name
			x=db(db.likes.likedby==a).select()
			for i in range((len(x))):
					a=x[i].newshead
					if (x[i].likeflag)==1:
						te=db(db.news.heading==a).select()
					elif (x[i].dislikeflag)==1:
						te=db(db.news.heading==a).select()
						fg=te[0].rating+3
						db(db.news.heading==a).update(rating=fg)
			u=temp[0].first_name+' '+temp[0].last_name
			db(db.likes.likedby==u).delete()
			db(db.news.posted_by==u).delete()
			db(db.comments.comment_by==u).delete()				


			db(db.auth_user.id==form.vars.deleteid).delete()
			response.flash=T("account successfully deleted")
			redirect(URL('index'))
	return dict(s=s,r=r,d=d,form=form)


@auth.requires_login()
def add():
	h=auth.user.id
	t=db(db.article.id>0).select()
	form=SQLFORM.factory(db.Field('field_to_be_added','string',required=True))
	if h>1:
		response.flash=T("You dont have permission to add new categories")
	else:
		flag=0
		if form.process().accepted:
			for i in range(len(t)):
				if t[i].category==form.vars.field_to_be_added:
					flag=1
			if flag==1:
				response.flash=T("Category already exists")
				redirect(URL('index'))
			else:
				db.article.insert(category=form.vars.field_to_be_added)
				response.flash=T("New category added successfully")
				redirect(URL('index'))
	return dict(form=form,h=h)
def cat1():
	a=str(request.vars.p)
	test=db(db.news.heading==a).select(db.news.posted_by)
	h=0
	r=db(db.news.heading==a).select()
	n=SQLFORM.factory(db.Field('select',requires=IS_IN_SET(['like','dislike','edit','delete','comment'])))
	rating=db.news.rating
	
	c=auth.user.first_name+' '+auth.user.last_name
		
	typ=auth.user.id
	fg=[]
	if n.process().accepted:
		if n.vars.select=='like':
			fg=db((db.likes.newshead==a)&(db.likes.likedby==c)).select()
			if fg:
				for i in range(len(fg)):
					if fg[i].likeflag==1:
						db(db.news.heading==a).update(rating=rating-5)
						db((db.likes.newshead==a)&(db.likes.likedby==c)).update(likeflag=0)
						response.flash=T("like cancelled")
						redirect(URL('cat1?p=%s'%a))
					elif fg[i].dislikeflag==1:
						db(db.news.heading==a).update(rating=rating+8)
						db((db.likes.newshead==a)&(db.likes.likedby==c)).update(likeflag=1,dislikeflag=0)
						response.flash=T("dislike changed to like")
						redirect(URL('cat1?p=%s'%a))
					elif fg[i].likeflag==0 & fg[i].dislikeflag==0:
						db(db.news.heading==a).update(rating=rating+5)
						db((db.likes.newshead==a)&(db.likes.likedby==c)).update(likeflag=1)
						response.flash=T("liked")
						redirect(URL('cat1?p=%s'%a))
					elif fg[i].likeflag==1 & fg[i].dislikeflag==1:
						db(db.news.heading==a).update(rating=rating-5)
						db((db.likes.newshead==a)&(db.likes.likedby==c)).update(likeflag=0)
						response.flash=T("like cancelled")
						redirect(URL('cat1?p=%s'%a))

			
			
			else:
				hj=auth.user.first_name+' '+auth.user.last_name
				db.likes.insert(likedby=hj,newshead=a,likeflag=1,dislikeflag=0)
				db(db.news.heading==a).update(rating=rating+5)
				response.flash=T("successfully liked and added +5 to rating")
				redirect(URL('cat1?p=%s'%a))
		elif n.vars.select=='dislike':
			fg=db((db.likes.newshead==a)&(db.likes.likedby==c)).select()
			if fg:
				for i in range(len(fg)):
					if fg[i].dislikeflag==1:
						db(db.news.heading==a).update(rating=rating+3)
						db((db.likes.newshead==a)&(db.likes.likedby==c)).update(dislikeflag=0)
						response.flash=T("dislike cancelled")
						redirect(URL('cat1?p=%s'%a))
					elif fg[i].likeflag==0 & fg[i].dislikeflag==0:
						db(db.news.heading==a).update(rating=rating-3)
						db((db.likes.newshead==a)&(db.likes.likedby==c)).update(dislikeflag=1)
						response.flash=T("disliked")
						redirect(URL('cat1?p=%s'%a))
					elif fg[i].likeflag==1 & fg[i].dislikeflag==1:
						db(db.news.heading==a).update(rating=rating+3)
						db((db.likes.newshead==a)&(db.likes.likedby==c)).update(dislikeflag=0)
						response.flash=T("dislike cancelled")
						redirect(URL('cat1?p=%s'%a))
					elif fg[i].likeflag==1:
						db(db.news.heading==a).update(rating=rating-8)
						db((db.likes.newshead==a)&(db.likes.likedby==c)).update(dislikeflag=1,likeflag=0)
						response.flash=T("like changed to dislike")
						redirect(URL('cat1?p=%s'%a))

			else:
				hj=auth.user.first_name+' '+auth.user.last_name
				db.likes.insert(likedby=hj,newshead=a,dislikeflag=1,likeflag=0)

				db(db.news.heading==a).update(rating=rating-3)
				response.flash=T("successfully disliked and added -3 to rating")
				redirect(URL('cat1?p=%s'%a))
		elif n.vars.select=='edit':
			if test[0].posted_by==c or typ==1:
				redirect(URL('edit?p=%s'%a))
			else:
				response.flash=T("Only owner/admin can edit")
				
		elif n.vars.select=='delete':
			if test[0].posted_by==c or typ==1:
				db(db.news.heading==a).delete()
				response.flash=T("successfully deleted")
				redirect(URL('index'))
			else:
				response.flash=T("Only owner/admin can delete")
		elif n.vars.select=='comment':
			redirect(URL('comment?p=%s'%a))
	print r[0].videourl
	return dict(test=test,a=a,n=n,r=r,h=h,fg=fg)
def comment():
	a=str(request.vars.p)
	x=db(db.news.heading==a).select()
	j=x[0]
	hj=auth.user.first_name+' '+auth.user.last_name
	form=SQLFORM.factory(db.Field('comment_body','text',required=True),
			db.Field('comment_by','string',default=hj),
			db.Field('news_heading','string',default=x[0].heading),
			db.Field('news_category','string',default=x[0].category),
			db.Field('news_url','string',default=x[0].url),
			db.Field('news_postedby','string',default=x[0].posted_by)
			)
	if form.process().accepted:
		db.comments.insert(comment_by=form.vars.comment_by,comment_body=form.vars.comment_body,news_heading=form.vars.news_heading,news_category=form.vars.news_category,news_url=form.vars.news_url,news_postedby=form.vars.news_postedby)
		response.flash=T("successfully commented")
		redirect(URL('comment?p=%s'%a))
	y=db(db.comments.news_heading==a).select()
	return dict(j=j,form=form,y=y,x=x)


def edit():
	a=str(request.vars.p)
	x=db(db.news.heading==a).select()
	form=SQLFORM.factory(db.Field('heading','string',default=a,required=True),
			db.Field('category','string',db.article,requires=IS_IN_DB(db,'article.category','article.category'),default=x[0].category,required=True),
			db.Field('url','string',default=x[0].url,required=True))
	if form.process().accepted:
		db(db.news.id==x[0].id).update(heading=form.vars.heading,category=form.vars.category,url=form.vars.url)
		response.flash=T("successfully edited")
		redirect(URL('index'))
		

	return dict(a=a,form=form)
	



@auth.requires_login()
def post():
	form=SQLFORM.factory(db.Field('category','string',db.article,requires=IS_IN_DB(db,'article.category','article.category'),required=True),
			db.Field('heading','string',required=True),
			db.Field('url','string'),
			db.Field('videourl','string'))
	if form.process().accepted:
		db.news.insert(category=form.vars.category,heading=form.vars.heading,url=form.vars.url,videourl=form.vars.videourl)
		response.flash=T("posted successfully")
		redirect(URL('index'))
	return dict(form=form)

@auth.requires_login()
def cat():
	a=str(request.vars.p)
	r=db(db.news.category==a).select(orderby=~db.news.rating)
	rank=db(db.news.category==a).select(db.news.heading,db.news.rating,orderby=~db.news.rating)
	l=FORM( INPUT(_type='submit',_value='like'))
	h=0
	if l.process().accepted:
		response.flash = T("successfully")
		h=56
	m=FORM( INPUT(_type='submit',_value='dislike'))
	return dict(a=a,r=r,l=l,m=m,h=h,rank=rank)


def user():
    """
    exposes:
    http://..../[app]/default/user/login
    http://..../[app]/default/user/logout
    http://..../[app]/default/user/register
    ettp://..../[app]/default/user/profile
    http://..../[app]/default/user/retrieve_password
    http://..../[app]/default/user/change_password
    use @auth.requires_login()
        @auth.requires_membership('group name')
        @auth.requires_permission('read','table name',record_id)
    to decorate functions that need access control
    """
    return dict(form=auth())


def download():
    """
    allows downloading of uploaded files
    http://..../[app]/default/download/[filename]
    """
    return response.download(request, db)


def call():
    """
    exposes services. for example:
    http://..../[app]/default/call/jsonrpc
    decorate with @services.jsonrpc the functions to expose
    supports xml, json, xmlrpc, jsonrpc, amfrpc, rss, csv
    """
    return service()


@auth.requires_signature()
def data():
    """
    http://..../[app]/default/data/tables
    http://..../[app]/default/data/create/[table]
    http://..../[app]/default/data/read/[table]/[id]
    http://..../[app]/default/data/update/[table]/[id]
    http://..../[app]/default/data/delete/[table]/[id]
    http://..../[app]/default/data/select/[table]
    http://..../[app]/default/data/search/[table]
    but URLs must be signed, i.e. linked with
      A('table',_href=URL('data/tables',user_signature=True))
    or with the signed load operator
      LOAD('default','data.load',args='tables',ajax=True,user_signature=True)
    """
    return dict(form=crud())
