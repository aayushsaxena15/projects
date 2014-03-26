# -*- coding: utf-8 -*-
# this file is released under public domain and you can use without limitations

#########################################################################
## This is a samples controller
## - index is the default action of any application
## - user is required for authentication and authorization
## - download is for downloading files uploaded in the db (does streaming)
## - call exposes all registered services (none by default)
#########################################################################

import os.path
def index():
    if auth.user:
	    redirect(URL('homeuser'))
    """
    example action using the internationalization operator T and flash
    rendered by views/default/index.html or views/generic.html

    if you need a simple wiki simple replace the two lines below with:
    return auth.wiki()
    """
    response.flash = T("Welcome to Project-X!")
    return dict(message=T('Hello World'))


def contact():
	return locals()

def about():
	return locals()
def recentpage():
	s=db(db.Project.id>0).select(db.Project.Project_Name,db.Project.Date_of_Submission,orderby=db.Project.Date_of_Submission)

	t=db(db.comments.id>0).select(db.comments.projectid,db.comments.commenttime,db.comments.comment_by,orderby=db.comments.commenttime)
	
	
	return dict(s=s,t=t)

def user():
    """
    exposes:
    http://..../[app]/default/user/login
    http://..../[app]/default/user/logout
    http://..../[app]/default/user/register
    http://..../[app]/default/user/profile
    http://..../[app]/default/user/retrieve_password
    http://..../[app]/default/user/change_password
    use @auth.requires_login()
        @auth.requires_membership('group name')
        @auth.requires_permission('read','table name',record_id)
    to decorate functions that need access control
    """
    return dict(form=auth())
@auth.requires_login()
def homeuser():
    s=db(auth.user.id==db.auth_user.id).select(db.auth_user.first_name,db.auth_user.last_name,db.auth_user.email,db.auth_user.username,db.auth_user.Typeofuser,db.auth_user.Organisation)
    pro=db(db.Project.Username==auth.user.id).select(db.Project.ALL)
    s=s[0]
    y=db(auth.user.id==db.image.imageowner).select(db.image.ALL)
    if y:
        y=y[0]
    else:
	y=None    
    response.flash="click on image to update it"
    return dict(s=s,y=y,pro=pro)
@auth.requires_login()
def home():
    s=db(auth.user.id==db.auth_user.id).select(db.auth_user.first_name,db.auth_user.last_name,db.auth_user.email,db.auth_user.username,db.auth_user.Typeofuser,db.auth_user.Organisation)
    pro=db(db.Project.Username==auth.user.id).select(db.Project.ALL)
    s=s[0]
    y=db(auth.user.id==db.image.imageowner).select(db.image.ALL)
    if y:
        y=y[0]
    else:
	y=None    
    response.flash="click on image to update it"
    return dict(s=s,y=y,pro=pro)
@auth.requires_login()
def showproject():
	a=request.vars.Projectid
	y=db(db.rating.rated_by==auth.user.id).select(db.rating.id)
    	pro=db(db.Project.id==a).select(db.Project.ALL)
	pro=pro[0]
	b=db(db.rating.ratedproject==a).select(db.rating.ALL)
	ans=0
	al=len(b)
	if b :
		for aabhas in b:
			ans=ans+float(aabhas.ratevalue)
		ans=ans/len(b)
	else:
		ans="unrated"
	comm=db(db.comments.projectid==a).select(db.comments.commentbody,db.comments.comment_by)
	form=SQLFORM.factory(db.Field('rating_by_you','string',required=False,requires=IS_IN_SET(['unchanged','0','1','2','3','4','5']),default="unchanged"),
			db.Field('comment','text',required=False,label='write your comment'))
	if form.process().accepted:
		if form.vars.rating_by_you and form.vars.rating_by_you!="unchanged":
			if y:
				for i in y:
					db(db.rating.id==i.id).delete()
			aa=db(db.Project.id==a).select(db.Project.Username)
			aa=aa[0]
			if aa.Username==auth.user.id:
				response.flash="You cannot rate your own Project"
			else:
				db.rating.insert(ratevalue=form.vars.rating_by_you,ratedproject=a)
				if form.vars.comment:
					db.comments.insert(commentbody=form.vars.comment,projectid=a)
				redirect(URL('showproject?Projectid=%s'%a))	
				response.flash="comment recorded"
		if form.vars.rating_by_you=="unchanged":
			if form.vars.comment:
				db.comments.insert(commentbody=form.vars.comment,projectid=a)
			redirect(URL('showproject?Projectid=%s'%a))	
			response.flash="comment recorded"

	return dict(pro=pro,form=form,ans=ans,comm=comm)
@auth.requires_login()
def showuser():
    a=int(request.vars.userid)
    if a==auth.user.id:
        redirect(URL('homeuser'))
    s=db(a==db.auth_user.id).select(db.auth_user.first_name,db.auth_user.last_name,db.auth_user.email,db.auth_user.username,db.auth_user.Typeofuser,db.auth_user.Organisation)
    #pro=db(db.Project.Username==a).select(db.Project.ALL)
    pro=db(db.Project.Username==a).select(db.Project.Project_Name,db.Project.id,db.Project.Revision_no,orderby=~db.Project.Revision_no)
    s=s[0]
    y=db(a==db.image.imageowner).select(db.image.ALL)
    if y:
        y=y[0]
    else:
	y=None    
    response.flash="hi!"
    return dict(s=s,y=y,pro=pro)
@auth.requires_login()
def imageupdate():
	y=db(auth.user.id==db.image.imageowner).select(db.image.ALL)
	form=SQLFORM(db.image)
	if form.process().accepted:
		if y:
			for i in y:
				db(db.image.id==i.id).delete()
		response.flash="response recorded"
		redirect(URL('homeuser'))
	return dict(form=form)
@auth.requires_login()
def createproject():
    s=SQLFORM(db.Project)
    if s.process().accepted:
        response.flash="response recorded"
	db.projectauth.insert(userallowed=auth.user.id,projectid=s.vars.id)
        redirect(URL('homeuser'))
    else:
        response.flash="create a project"
    return dict(s=s)
@auth.requires_login()
def reviseproject():
	aayush=db((db.projectauth.userallowed==auth.user.id)&(db.projectauth.projectid==db.Project.id)).select(db.Project.Project_Name)
	req=[]
	for aabhas in aayush:
		req.append(aabhas.Project_Name)
	s=SQLFORM.factory(Field('projectname',requires=IS_IN_SET(req)))
	if s.process().accepted:
		redirect(URL('version1?projectid=%s'%s.vars.projectname))
	return dict(s=s,a=aayush)
def version1():
	p=request.vars.projectid
	a=db(p==db.Project.Project_Name).select(orderby=~db.Project.Revision_no)
	b=a[0]
	form1=SQLFORM(db.Project)
	form1.vars.Project_Name=b.Project_Name
	form1.vars.Revision_no=b.Revision_no+1
	form1.vars.Username=b.Username
	form1.vars.Desription=b.Desription
	form1.vars.Type_of_Project=b.Type_of_Project
	form1.vars.Developed_as=b.Developed_as
	form1.vars.Project_Mode=b.Project_Mode
	if form1.accepts(request.vars,session):
		if not(form1.vars.Project_File):
			db(form1.vars.id==db.Project.id).update(Project_File=b.Project_File)
		if not(form1.vars.Project_Manual):
			db(form1.vars.id==db.Project.id).update(Project_Manual=b.Project_Manual)
	return dict(a=a,form1=form1)
def download():
    """
    allows downloading of uploaded files
    http://..../[app]/default/download/[filename]
    """
    return response.download(request, db)

def download1():
    """
    allows downloading of uploaded files
    http://..../[app]/default/download/[filename]
    """
    #t=request.vars.arg(0)
    response.flash=request
    #print request.wsgi.environ['HTTP_REFERER']
    #print 'yghklo=',request.args[0]
    a=db(db.Project.Project_File==request.args[0]).select(db.Project.ALL)
    #a=db(db.Project.id==38).select(db.Project.ALL)
    #if a == None:
#	    print 'silent'
 #   print 'a=  aabhas download',a[0].no_of_download, a[0].Project_File
   # if a[0].no_of_download==None:
#	    a[0].no_download=0
    db(db.Project.Project_File==a[0].Project_File).update(no_of_download=(a[0].no_of_download or 0)+1)
    print 'a.id=',a[0].id
   # print len(a),'\n'
    #print "\n\n\n\n"
    return response.download(request, db)
@auth.requires_login()
def searchproject():
	f=SQLFORM.factory(Field('name','string',label='Enter Project Name/Decription To Be Searched'))
	if f.process().accepted:
		redirect(URL('getproject?search=%s'%f.vars.name))
	return dict(f=f)
@auth.requires_login()
def getproject():
	b=request.vars.search
	a='%'+b+'%'
	y=db(db.Project.Project_Name.like(a)).select()
	x=db(db.Project.Desription.like(a)).select()
	return dict(y=y,b=b,x=x)
@auth.requires_login()
def searchuser():
	f=SQLFORM.factory(Field('name','string',label='Enter Username To Be Searched'))
	if f.process().accepted:
		redirect(URL('getuser?search=%s'%f.vars.name))
	return dict(f=f)
@auth.requires_login()
def getuser():
	b=request.vars.search
	a='%'+b+'%'
	y=db(db.auth_user.username.like(a)).select()
	return dict(y=y,b=b)
def call():
    return service()

def search():
	a=''
	y=db(db.auth_user.username.like(a)).select()
	return dict(y=y)
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
