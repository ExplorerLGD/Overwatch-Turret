from maya import cmds
import json
import time
import thread
import threading
import maya.utils as utils
import socket
#some settings --------
frameRate=16
times=[0,23]
filePath='E:/robot01/Program/UploadAnim/data/test.txt'

sendRate=0.03
isTest=False
#----------------------
selection=cmds.ls(selection=True,long=True)

def getKeyframe(selection):
    result=[]
    for obj in selection:
        print obj
        a=cmds.keyframe(obj,time=(times[0],times[1]),query=True,timeChange=True)
        print a
        #如果没有K帧，则赋值0，防止NoneType
        if(a==None):
            a=[0.0]
        #使用set求并集
        result=set(result)|set(a)
        result=list(result)
    result.sort()
    print result
    return result

def getData(keys,selection,frameRate):
    data=[]
    for index in range(len(keys)):
        dict={}
        if keys[index]==0.0:
            time=1/float(frameRate)
        else:
            time=(keys[index]-keys[index-1])/frameRate

        dict['time']=time
        objects={}
        for obj in selection:
            shortName = obj.split('|')[-1]
            attrs=cmds.listAttr(obj,u=True,k=True)
            

            s=str(obj+'.'+attrs[0])
            d=int(cmds.getAttr(s,time=keys[index]))
            objects[shortName]=d
        dict['objects']=objects
        data.append(dict)
    jsonData=json.dumps(data)
    return jsonData

class cThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        #self.function
        #utils.executeDeferred(self.function)
        sendData()
        
def getCurrentData():
    data=[]
    dataDict={}
    dataDict['time']=0.0
    objects={}
    for obj in selection:
        shortName = obj.split('|')[-1]
        print shortName
        attrs=cmds.listAttr(obj,u=True,k=True)
        s=str(obj+'.'+attrs[0])
        d=int(cmds.getAttr(s))
        objects[shortName]=d
        print "attrs: "+attrs[0]
    dataDict['objects']=objects
    data.append(dataDict)
    jsonData=json.dumps(data)
    print jsonData
    return jsonData
   
def sendData():
    cmds.progressWindow(isInterruptable=1)
    #client = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    #client.connect(('192.168.0.100',6969))
    while 1 :
        if cmds.progressWindow(query=1, isCancelled=1) :
            client.close()
            break
        print "do something"
        data=utils.executeInMainThreadWithResult(getCurrentData)
        data="<"+data+">"
        client.send(data)
        time.sleep(sendRate)
    cmds.progressWindow(endProgress=1)

  
if isTest:
    client = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    client.connect(('192.168.0.106',9696))
    try:
        #thread.start_new_thread(sendData(),)
        mythread=cThread()
        mythread.start()
    except:
        print "unable to start thread"
    #mythread=cThread()
    #mythread.start()
    print "start"


else:                
    keys=getKeyframe(selection)
    
    data=getData(keys,selection,frameRate)
    print data     
    print "write to file"
    with open(filePath,'w') as file:
        file.write(data)
    
