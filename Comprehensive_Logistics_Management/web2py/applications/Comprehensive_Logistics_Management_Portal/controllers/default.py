# -*- coding: utf-8 -*-
# this file is released under public domain and you can use without limitations

#########################################################################
## This is a sample controller
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
    return dict(message=T('Hello World'))


def user():
    """
    exposes:
    http://..../[app]/default/user/login
    http://..../[app]/default/user/logout
    http://..../[app]/default/user/register
    http://..../[app]/default/user/profile
    http://..../[app]/default/user/retrieve_password
    http://..../[app]/default/user/change_password
    http://..../[app]/default/user/manage_users (requires membership in 
    use @auth.requires_login()
        @auth.requires_membership('group name')
        @auth.requires_permission('read','table name',record_id)
    to decorate functions that need access control
    """
    return dict(form=auth())

@cache.action()
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
@auth.requires_login()
def place_order():
    if request.post_vars['source_city']:
        """print request.post_vars;
        print request.post_vars['service']
        print request.post_vars['source_city']
        print request.post_vars['Destination_city']
        """
        a=db((db.Location1.Source_City==request.post_vars['source_city'])&( db.Location1.Destination_City==request.post_vars['destination_city']) ).select()
        #print a[0].id
        print request.post_vars['type']
        db.Logistics.insert(Service=request.post_vars['service'],
            location_id=a[0].id,
            Logistic_Type=request.post_vars['type'],
            Mode_of_Payment=request.post_vars['mode_of_payment'],
            Weight=request.post_vars['weight'],
            customer_id=auth.user.id,
            Source_address=request.post_vars['source_address'],
            Destination_address=request.post_vars['destination_address'])
        session.flash='Order Placed'
        mail_body='Hi ,%s\nYou have successfully placed order for your consignment\nDetails are :\nThank You'%(auth.user.first_name,)
        mail.send('sksaurabhkathpalia@gmail.com', 'Booking Confirmation Mail', mail_body)
        redirect(URL('index'))
    return locals()

def track_consignment():
    consignment_id=request.vars.consignment_id
    x=db(db.Logistics.id==consignment_id).select(db.Logistics.ALL)
    lid=x[0]['location_id']
    loc=db(db.Location1.id==lid).select(db.Location1.ALL)
    return dict(x=x,loc=loc)

def estimate_cost():
    return locals()

def estimate():
    source_city=request.vars.source_city;
    destination_city=request.vars.destination_city;
    print source_city
    print destination_city
    logistic_type=request.vars.logistic_type
    weight=request.vars.weight
    if(logistic_type=='Document'):
        weight=0
    s="<h3>Enter Weight</h3>"
    if(weight==""):
        return s
    mode_of_payment=request.vars.mode_of_payment
    service=request.vars.service
    print service
    print logistic_type,mode_of_payment
    c1="<h3>Estimated Cost of consignment &nbsp;&nbsp; "
    cost=db((db.Location1.Source_City==source_city )&(db.Location1.Destination_City==destination_city)).select(db.Location1.standard_cost)
    cost=(cost[0]['standard_cost'])
    if(weight!=None):
        weight=int(weight)
    if(logistic_type=='Non-Document'):
        cost+=weight*25
    if(mode_of_payment!='Self Service(You go to dealer to pay)'):
        cost+=30
    if(service=='Airport to Airport'):
        cost+=50
    elif (service=='Domestic Priority'):
        cost+=30
    elif(service=='Domestic Priority 1030'):
        cost+=35
    elif(service=='Door to Door'):
        cost+=30
    cost="<br/><center>Rs. "+str(cost) +"</center>"
    c1+=cost
    cost=c1
    #print c2
  #  ret="<script>" + "function cost() { alert('"
    ret=""
    ret+=cost
   # ret+="');"
   # ret+="}"
  #  ret+="cost();"
  #  ret+="</script>"
    ret+="</h3>"
    print ret
    return ret

def enter_id():
    return locals();

def check_dest():
    source_city=request.vars.source_city;
 #   print source_city
   # soure_city=source_city[0]
    a=[]
    ret=db(db.Location1.Source_City==source_city).select(db.Location1.Destination_City)
    for i in ret:
        a.append(i['Destination_City'])
    a=set(a)
    result=""
    result+="<link rel=\"stylesheet\" href=\"/Comprehensive_Logistics_Management_Portal/static/chosen/chosen.css\"><style type=\"text/css\" media=\"all\">.chosen-rtl .chosen-drop { left: -9000px; }</style>"
    result+="<select class=\"chosen-select\" name=\"destination_city\">"
    for i in a:
        result += "<option value=\"" + str(i) + "\"" + ">" + i + "</option>"
    result+="</select>"
    #result+="<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.6.4/jquery.min.js\" type=\"text/javascript\"></script>
    result+="<script src=\"/Comprehensive_Logistics_Management_Portal/static/chosen/chosen.jquery.js\" type=\"text/javascript\"></script>"
    result+="  <script type=\"text/javascript\">var config = {'.chosen-select': {},'.chosen-select-deselect'  : {allow_single_deselect:true},'.chosen-select-no-single' : {disable_search_threshold:10}, '.chosen-select-no-results': {no_results_text:'Oops, nothing found!'},'.chosen-select-width': {width:\"95%\"}}\n  for (var selector in config) {$(selector).chosen(config[selector]);}</script>"  
    #print result
    return result



def adminupdate():
    v=request.post_vars['submit']
    print v;
    if v:
        #print request.post_vars;
        #print request.post_vars['current'],request.post_vars['expected'];
        cur=request.post_vars['current']
        order=request.post_vars['orderid']
        d=request.post_vars['expected']        
        print d,cur,order;
        db(db.Logistics.id==order).update(Expected_Delivery_Date=d,Current_location=cur)  
        response.flash="updated"
    return locals()



def showorderid():
    #pint 'aabhas'
    var=request.vars.orderid;
    a=db(db.Logistics.id==var).select()
    #print a
    s=""
    if len(a)==0:
        s="Enter a valid id"
    else:
        a=a[0]
        s=""
        s+="<b>id : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+str(a.id) + "<br>"
        s+="<b>Source_address : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + str(a.Source_address) +"<br/>"
        s+="<b>Destination_address : </b>&nbsp;" + str(a.Destination_address) +"<br/>"
        s+="<b>Logistic_Type : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" +str(a.Logistic_Type) +"<br/>"
        if a.Logistic_Type!='Document':
            print 'aabhas_majumdar'
            s+="<b>Weight : </b>&nbsp;&nbsp;&nbsp;&nbsp;" +str(a.Weight) +"<br/>"
        s+="<b>Mode_of_Payment : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" +str(a.Mode_of_Payment) +"<br/>"
        s+="<b>Service : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" +str(a.Service) +"<br/>"
        loc=db(db.Location1.id==a.location_id).select()
        loc=loc[0];
        s+="<b>Destination city : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + str(loc.Destination_City) +"<br/>"
        s+="<b>Source City : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + str(loc.Source_City) +"<br/>"
        cus=db(a.customer_id==db.auth_user.id).select()
        s+="<b>Username : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + str(cus[0].username) +"<br/>" 
        s+="<b>Current Location : </b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" +str(a.Current_location) +"<br/>"
        s+="<b>Expected Delivery Date </b>:" +str(a.Expected_Delivery_Date) +"<br/>"
        #s+="<form method =\"POST\" action=\"\" >"
        #s+="<form action=\"#\" enctype=\"multipart/form-data\" method=\"post\">"
        if a.Current_location==None:
            a.Current_location=''
        s+="<b>Current Location &nbsp;&nbsp; : &nbsp;&nbsp;</b><input value=\"%s\" name=\"current\" type=\"text\" />"%a.Current_location
        if a.Expected_Delivery_Date==None:
            a.Expected_Delivery_Date=''
        #s+="<br/> Expected Delivery Date&nbsp;&nbsp; : &nbsp;&nbsp;<input class=\"date\" placeholder=\"yyyy-mm-dd\" type=\'text\' value=\"%s\" name=\"expected\" />"%a.Expected_Delivery_Date
        #s+="<tr id=\>"Logistics_Expected_Delivery_Date__row\"><td class=\"w2p_fl\"><label for=\"Logistics_Expected_Delivery_Date\" id=\"Logistics_Expected_Delivery_Date__label\">Expected Delivery Date: </label></td><td class=\"w2p_fw\"><input class=\"date\" id=\"Logistics_Expected_Delivery_Date\" name=\"Expected_Delivery_Date\" value=\"\" type=\"text\"></td><td class=\"w2p_fc\"></td></tr>"
        s+="<script src=\"/Comprehensive_Logistics_Management_Portal/static/js/jquery.js\" type=\"text/javascript\"></script><link href=\"/Comprehensive_Logistics_Management_Portal/static/css/calendar.css\" rel=\"stylesheet\" type=\"text/css\" /><script src=\"/Comprehensive_Logistics_Management_Portal/static/js/calendar.js\" type=\"text/javascript\"></script><script src=\"/Comprehensive_Logistics_Management_Portal/static/js/web2py.js\" type=\"text/javascript\"></script><link href=\"/Comprehensive_Logistics_Management_Portal/static/css/bootstrap-responsive.min.css\" rel=\"stylesheet\" type=\"text/css\" /><link href=\"/Comprehensive_Logistics_Management_Portal/static/css/web2py_bootstrap.css\" rel=\"stylesheet\" type=\"text/css\" />"
        s+="<table><tr><td class=\"w2p_fc\"></td></tr><tr id=\"Logistics_Expected_Delivery_Date__row\"><td class=\"w2p_fl\"><label for=\"Logistics_Expected_Delivery_Date\" id=\"Logistics_Expected_Delivery_Date__label\"><b>Expected Delivery Date: </b></label></td><td class=\"w2p_fw\"><input class=\"date\" id=\"expected\" name=\"expected\" type=\"text\" value=\"%s\" /></td></table>"%a.Expected_Delivery_Date
        
        #s+="<table>"
        s+="<br/><input type=\"submit\" name=\'submit\' value=\"update\" class=\"btn btn-primary / \">"
        #s+="</div>"
        #s+="</table>"
        #s+="</form>"
        #s+= "" + str(a.id) +"<br/>"
    form=SQLFORM(db.Logistics)
    return  XML(s)
    #return dict(form=form)


def showorderid_1():
    #pint 'aabhas'
    var=request.vars.orderid;
    a=db(db.Logistics.id==var).select()
    #print a
    s=""
    if len(a)==0:
        s="Enter a valid id"
    else:
        a=a[0]
        loc=db(db.Location1.id==a.location_id).select()
        loc=loc[0];
        cus=db(a.customer_id==db.auth_user.id).select()
        s+="Username :" + str(cus[0].username) +"<br/>" 
        s+="Source City :" + str(loc.Source_City) +"<br/>"
        s+="Destination city :" + str(loc.Destination_City) +"<br/>"
        
        s+="Current Location :" +str(a.Current_location) +"<br/>"
        s+="Expected Delivery Date :" +str(a.Expected_Delivery_Date) +"<br/>"
        #s+="<form method =\"POST\" action=\"\" >"
        #s+="<form action=\"#\" enctype=\"multipart/form-data\" method=\"post\">"
    
    return  XML(s)
    #return dict(form=form)



@auth.requires_login()
def view_consignment():
    a=db(db.Logistics.customer_id==auth.user.id).select()
    b='aabhas'
    return dict(a=a)


def test():
    form=SQLFORM(db.Logistics)
    a=1
    print dir(form.hidden_fields);
    if form.process().accepted and a==  0:
        print dir(form)
        print 
        print dir(form.vars)
        print '\n',form.vars;
    return locals()



@auth.requires_login()
def testmail():
    print auth.user.email;
    mail = auth.settings.mailer
    mail.settings.server = 'aabhas_majumdar@yahoo.com:25'
    mail.settings.sender = 'admin@logistic.com'
    mail.settings.login = 'admin:password'
    a=mail.send(to=['noreply@example.com'],
          subject='hello',
          # If reply_to is omitted, then mail.settings.sender is used
          reply_to='us@example.com',
          message='hi there')
    print a;
    return locals()


def cancel_consignment():
    x=request.vars.cancel_id
    print x
    db(db.Logistics.id==x).delete()
    redirect(URL('default','view_consignment'))



def adminadd():
    form=SQLFORM(db.Location1)
    if form.process().accepted:
        session.flash="new location added"
        redirect(URL('index'))
    return locals()

def contact():
    return locals()


def about_us():
    return locals()


def track_by_id():
    order_id=request.vars.order_id
    x=db(db.Logistics.id==order_id).select(db.Logistics.ALL)
    print x
    return dict(x=x)
