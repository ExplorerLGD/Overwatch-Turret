from maya import cmds
import json
import time
import threading
import maya.utils as utils
import socket
#some settings --------
frameRate=16
times=[0,24]
filePath='C:/Users/Administrator/Desktop/data.txt'

sendRate=0.5
isTest=True
#----------------------
selection=cmds.ls(selection=True,long=True)

def getKeyframe(selection):
    result=[]
    for obj in selection:
        a=cmds.keyframe(obj,time=(times[0],times[1]),query=True,timeChange=True)
        result=set(result)|set(a)
        result=list(result)
    result.sort()
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
            
            dictAttrs={}
            for attr in attrs:
                s=str(obj+'.'+attr)
                d=cmds.getAttr(s,time=keys[index])
                dictAttrs[attr]=d
            objects[shortName]=dictAttrs
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
    client.connect(('192.168.0.100',9696))
    mythread=cThread()
    mythread.start()
    print "start"


else:                
    keys=getKeyframe(selection)
    
    data=getData(keys,selection,frameRate)
            
    with open(filePath,'w') as file:
        json.dump( data, file, indent=4, sort_keys=True)
    
