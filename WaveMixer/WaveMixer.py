import Tkinter
import math
import pyaudio
import os
import wave
import tkFileDialog
from Tkinter import *
import tkMessageBox
import time, wave
import pygame

import struct
from Tkinter import Checkbutton,Frame, Tk, BOTH, Text, Menu, END,IntVar
import tkFileDialog 
import signal
import contextlib

selected=[' ',' ',' ']
################################################################################################################3
filename1=''
filename2=''
filename3=''
p1=()
new_data=[]
lis1=[]
mod=[None]*3
mix=[None]*3
a=[None]*3
r=[None]*3
tsc=[None]*3
tsh=[None]*3
int_data3=[]


from sys import byteorder
from array import array
import struct

import pyaudio
import wave

class RECORD:
	def __init__(self):
		self.THRESHOLD = 500
		self.CHUNK_SIZE = 1024
		self.FORMAT = pyaudio.paInt16
		self.RATE = 44100

	def is_silent(self,snd_data):
		"Returns 'True' if below the 'silent' threshold"
		return max(snd_data) < self.THRESHOLD

	def normalize(self,snd_data):
		"Average the volume out"
		MAXIMUM = 16384
		times = float(MAXIMUM)/max(abs(i) for i in snd_data)
		r = array('h')
		for i in snd_data:
			r.append(int(i*times))
		return r

	def trim(self,snd_data):
		"Trim the blank spots at the start and end"
		def _trim(snd_data):
			snd_started = False
			r = array('h')
			for i in snd_data:
				if not snd_started and abs(i)>self.THRESHOLD:
					snd_started = True
					r.append(i)
				elif snd_started:
					r.append(i)
			return r
		snd_data = _trim(snd_data)
		snd_data.reverse()
		snd_data = _trim(snd_data)
		snd_data.reverse()
		return snd_data

	def add_silence(self,snd_data, seconds):
		"Add silence to the start and end of 'snd_data' of length 'seconds' (float)"
		r = array('h', [0 for i in xrange(int(seconds*self.RATE))])
		r.extend(snd_data)
		r.extend([0 for i in xrange(int(seconds*self.RATE))])
		return r

	def record(self):
		p = pyaudio.PyAudio()
		stream = p.open(format=self.FORMAT, channels=1, rate=self.RATE,input=True, output=True,frames_per_buffer=self.CHUNK_SIZE)
		num_silent = 0
		snd_started = False
		r = array('h')
		while 1:
			snd_data = array('h', stream.read(self.CHUNK_SIZE))
			if byteorder == 'big':
				snd_data.byteswap()
			r.extend(snd_data)
			silent = self.is_silent(snd_data)
			if silent and snd_started:
				num_silent += 1
			elif not silent and not snd_started:
				snd_started = True
			if snd_started and num_silent > 3:
				break
		sample_width = p.get_sample_size(self.FORMAT)
		stream.stop_stream()
		stream.close()
		p.terminate()
		r = self.normalize(r)
		r = self.trim(r)
		r = self.add_silence(r, 0.5)
		return sample_width, r
	def record_to_file(self,path):
		sample_width, data = self.record()
		data = struct.pack('<' + ('h'*len(data)), *data)
		wf = wave.open(path, 'wb')
		wf.setnchannels(1)
		wf.setsampwidth(sample_width)
		wf.setframerate(self.RATE)
		wf.writeframes(data)
		wf.close()







def read(Fname,int_data):
	w=wave.open(Fname,'rb')
	channel=w.getnchannels()
	samplerate=w.getframerate()
	samplewidth=w.getsampwidth()
	frames=w.getnframes()
	raw_data=w.readframes(frames)
	w.close()

	samples=frames*channel
	if samplewidth==1:
		fmt="%iB" %samples
	elif samplewidth==2:
		fmt="%ih" %samples
	new_data=list(struct.unpack(fmt,raw_data))
	int_data=new_data

class mixer():
	def __init__(self,Fname):
		w=wave.open(Fname,'rb')
		self.channel=w.getnchannels()
		self.samplerate=w.getframerate()
		self.samplewidth=w.getsampwidth()
		self.frames=w.getnframes()
		self.raw_data=w.readframes(self.frames)
		w.close()

		self.samples=self.frames*self.channel
		if self.samplewidth==1:
			fmt="%iB" %self.samples
		elif self.samplewidth==2:
			fmt="%ih" %self.samples
		self.new_data=list(struct.unpack(fmt,self.raw_data))
		self.int_data=self.new_data

	def shift(self,by):
		framestoshift=int(by*self.samplerate)
		if framestoshift<0:
			framestoshift=-framestoshift
		if by<0:
			if self.channel==1:
				self.new_data=self.new_data[framestoshift::1]
			else:
				self.new_data=self.new_data[2*framestoshift::1]
		else:
			if self.channel==1:
				extra=[]
				for i in range(framestoshift):
					extra.append(0)
				self.new_data=extra+self.new_data
			else:
				extra=[]
				for i in range(2*framestoshift):
					extra.append(0)
				self.new_data=extra+self.new_data
		self.frames=len(self.new_data)/self.channel

	def scale(self,scaleby):
		n=[]
		if scaleby!=0:
			if self.channel==1:
				for i in range(int(len(self.new_data)/scaleby)):
					n.append(self.new_data[int(scaleby*i)])
			else:
				n1=[]
				n2=[]

				for i in range(len(self.new_data)):
					if i%2==0:
						n2.append(self.new_data[i])
					else:
						n1.append(self.new_data[i])
				for i in range(int(len(n2)/scaleby)):
					n.append(n2[int(scaleby*i)])
					n.append(n1[int(scaleby*i)])

			if scaleby!=1:
				self.new_data=n
				self.frames=len(self.new_data)/self.channel




	def write1(self,output):
		if self.samplewidth==1:
			fmt="%iB" %self.frames*self.channel
		else:
			fmt="%ih" %self.frames*self.channel
		final_data=struct.pack(fmt,*(self.new_data))
		nw=wave.open(output,'w')
		nw.setframerate(self.samplerate)
		nw.setnframes(self.frames)
		nw.setsampwidth(self.samplewidth)
		nw.setnchannels(self.channel)
		nw.writeframes(final_data)
		nw.close()
	def amp1(self,qty):
		
		if(qty!=0):
			for i in xrange(len(self.new_data)):
				if self.new_data[i]*qty>32767:
					self.new_data[i]=32767
				elif qty*self.new_data[i] < -32768:
					self.new_data[i]=-32768
				else:
					self.new_data[i]=self.new_data[i]*qty

	def reverse1(self):
		global p1
		global lis1
		if self.channel==1:
			self.new_data.reverse()
		else:
			self.new_data.reverse()
			for i in xrange(len(self.new_data)-1):
				t=self.new_data[i]
				self.new_data=self.new_data[i+1]
				self.new_data[i+1]=t
filetomodulate=[None]*3
def modfunc():
	global mod
	global filetomodulate
	filetomodulate=[None]*3

	filestomodulate=[]
	for i in range(len(mod)):
		if mod[i]:
			filetomodulate[i]=mixer(mod[i])
			filetomodulate[i].amp1(a[i])
			filetomodulate[i].shift(tsh[i])
			filetomodulate[i].scale(tsc[i])
			if r[i]:
				filetomodulate[i].reverse1()

	length1=0
	length2=0
	length3=0
	if filetomodulate[0]:
		length1=len(filetomodulate[0].new_data)

	if filetomodulate[1]:
		length2=len(filetomodulate[1].new_data)
	if filetomodulate[2]:
		length3=len(filetomodulate[2].new_data)
	minl=max(length1,length2,length3)
	if filetomodulate[0]:
		if filetomodulate[0].new_data:
			last=0
	if filetomodulate[1]:
		if filetomodulate[1].new_data:
			last=1
	if filetomodulate[2]:
		if filetomodulate[2].new_data:
			last=2
	for i in range(length1,minl):
		if filetomodulate[0]:
			filetomodulate[0].new_data.append(0)
	for i in range(length2,minl):
		if filetomodulate[1]:
			filetomodulate[1].new_data.append(0)
	for i in range(length3,minl):
		if filetomodulate[2]:
			filetomodulate[2].new_data.append(0)

	if filetomodulate[0] and filetomodulate[1] and filetomodulate[2]:
		for i in range(minl):
			if filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]>32767:
				filetomodulate[last].new_data[i]=32767
			elif filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]<-32768:
				filetomodulate[last].new_data[i]=-32768
			elif filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]>=-32768 and filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]<=32767:
				filetomodulate[last].new_data[i]=filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]


	if filetomodulate[0] and filetomodulate[1] and filetomodulate[2]==None:
		for i in range(minl):
			if filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]>32767:
				filetomodulate[last].new_data[i]=32767
			elif filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]<-32768:
				filetomodulate[last].new_data[i]=-32768
			elif filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]>=-32768 and filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]<=32767:
				filetomodulate[last].new_data[i]=filetomodulate[0].new_data[i]*filetomodulate[1].new_data[i]


	if filetomodulate[0] and filetomodulate[2] and filetomodulate[1]==None:
		for i in range(minl):
			if filetomodulate[0].new_data[i]*filetomodulate[2].new_data[i]>32767:
				filetomodulate[last].new_data[i]=32767
			elif filetomodulate[0].new_data[i]*filetomodulate[2].new_data[i]<-32768:
				filetomodulate[last].new_data[i]=-32768
			elif filetomodulate[0].new_data[i]*filetomodulate[2].new_data[i]>=-32768 and filetomodulate[0].new_data[i]*filetomodulate[2].new_data[i]<=32767:
				filetomodulate[last].new_data[i]=filetomodulate[0].new_data[i]*filetomodulate[2].new_data[i]


	if filetomodulate[0]==None and filetomodulate[2] and filetomodulate[1]:
		for i in range(minl):
			if filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]>32767:
				filetomodulate[last].new_data[i]=32767
			elif filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]<-32768:
				filetomodulate[last].new_data[i]=-32768
			elif filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]>=-32768 and filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]<=32767:
				filetomodulate[last].new_data[i]=filetomodulate[1].new_data[i]*filetomodulate[2].new_data[i]



	filetomodulate[last].frames=minl/filetomodulate[last].channel
	filetomodulate[last].write1('modulated.wav')
	playf('modulated.wav')
	filename1=' '
	filename2=' '
	filename3=' '





filetomix=[None]*3
def mixfunc():
	global mix
	global filetomix
	filetomix=[None]*3
	last=-1
	for i in range(len(mix)):
		if mix[i]:
			filetomix[i]=mixer(mix[i])
			filetomix[i].amp1(a[i])
			filetomix[i].shift(tsh[i])
			filetomix[i].scale(tsc[i])
			if r[i]:
				filetomix[i].reverse1()
			last=i

	length1=0
	length2=0
	length3=0
	if filetomix[0]:
		length1=len(filetomix[0].new_data)
	if filetomix[1]:
		length2=len(filetomix[1].new_data)
	if filetomix[2]:
		length3=len(filetomix[2].new_data)
	maxl=max(length1,length2,length3)

	if filetomix[0]:
		for i in range(length1,maxl):
			filetomix[0].new_data.append(0)
	if filetomix[1]:
		for i in range(length2,maxl):
			filetomix[1].new_data.append(0)
	if filetomix[2]:
		for i in range(length3,maxl):
			filetomix[2].new_data.append(0)




######
	if filetomix[0] and filetomix[1] and filetomix[2]:	
		for i in range(maxl):
			if filetomix[0].new_data[i]+filetomix[1].new_data[i]+filetomix[2].new_data[i]>32767:
				filetomix[last].new_data[i]=32767
			elif filetomix[0].new_data[i]+filetomix[1].new_data[i]+filetomix[2].new_data[i]<-32768:
				filetomix[last].new_data[i]=-32768
			elif filetomix[0].new_data[i]+filetomix[1].new_data[i]+filetomix[2].new_data[i]>=-32768 and filetomix[0].new_data[i]+filetomix[1].new_data[i]+filetomix[2].new_data[i]<=32767:
				filetomix[last].new_data[i]=filetomix[0].new_data[i]+filetomix[1].new_data[i]+filetomix[2].new_data[i]

	if filetomix[0] and filetomix[1] and filetomix[2]==None:	
		for i in range(maxl):
			if filetomix[0].new_data[i]+filetomix[1].new_data[i]>32767:
				filetomix[last].new_data[i]=32767
			elif filetomix[0].new_data[i]+filetomix[1].new_data[i]<-32768:
				filetomix[last].new_data[i]=-32768
			elif filetomix[0].new_data[i]+filetomix[1].new_data[i]>=-32768 and filetomix[0].new_data[i]+filetomix[1].new_data[i]<=32767:
				filetomix[last].new_data[i]=filetomix[0].new_data[i]+filetomix[1].new_data[i]
######
	if filetomix[0] and filetomix[1]==None and filetomix[2]:	
		for i in range(maxl):
			if filetomix[0].new_data[i]+filetomix[2].new_data[i]>32767:
				filetomix[last].new_data[i]=32767
			elif filetomix[0].new_data[i]+filetomix[2].new_data[i]<-32768:
				filetomix[last].new_data[i]=-32768
			elif filetomix[0].new_data[i]+filetomix[2].new_data[i]>=-32768 and filetomix[0].new_data[i]+filetomix[2].new_data[i]<=32767:
				filetomix[last].new_data[i]=filetomix[0].new_data[i]+filetomix[2].new_data[i]
	if filetomix[0]==None and filetomix[1] and filetomix[2]:	
		for i in range(maxl):
			if filetomix[1].new_data[i]+filetomix[2].new_data[i]>32767:
				filetomix[last].new_data[i]=32767
			elif filetomix[1].new_data[i]+filetomix[2].new_data[i]<-32768:
				filetomix[last].new_data[i]=-32768
			elif filetomix[1].new_data[i]+filetomix[2].new_data[i]>=-32768 and filetomix[1].new_data[i]+filetomix[2].new_data[i]<=32767:
				filetomix[last].new_data[i]=filetomix[1].new_data[i]+filetomix[2].new_data[i]
	filetomix[last].frames=maxl/filetomix[last].channel
	filetomix[last].write1('mixed.wav')
	playf('mixed.wav')
	filename1=' '
	filename2=' '
	filename3=' '


def playf(out):
	Chunk=1024
	pid=os.fork()
	if pid==0:
		f = wave.open(out,'rb')  
		pl = pyaudio.PyAudio()  
		stream = pl.open(format = pl.get_format_from_width(f.getsampwidth()),channels = f.getnchannels(),rate = f.getframerate(), output = True)  
		data = f.readframes(Chunk)  

		while data != '':  
			stream.write(data)  
			data = f.readframes(Chunk)  
		stream.stop_stream()  
		stream.close()  
		pl.terminate()
		exit(0)



class setup():
	def __init__(self):
		self.pid1=0
		self.playing1=0
		self.paused1=0
		self.pid2=0
		self.playing2=0
		self.paused2=0
		self.pid3=0
		self.playing3=0
		self.paused3=0
		self.file1=''
		self.root=Tkinter.Tk()
		RWidth=self.root.winfo_screenwidth()
		RHeight=self.root.winfo_screenheight()
		self.root.geometry(("%dx%d")%(RWidth,RHeight))


		self.title=Label(self.root)
		self.title.pack()
		self.title.place(x=500,y=30)
		self.title.config(text ="WAVE MIXER")

		self.buttonbrl = Button(self.root, text="Select File", command=self.showl1)
		self.buttonbrl.pack()
		self.buttonbrl.place(x=120,y=170)
		self.browsel1=Label(self.root)
		self.browsel1.pack()
		self.browsel1.place(x=210,y=170)

		self.wave1=Label(self.root)
		self.wave1.pack()
		self.wave1.place(x=120,y=120)
		self.wave1.config(text ="WAVE 1")


		self.labelampl=Label(self.root)
		self.labelampl.pack()
		self.labelampl.place(x=120,y=220)
		self.labelampl.config(text ="Amplitude")
		self.l1=1
		
		self.scalel1=Scale(self.root,orient='horizontal',command=self.print_valuel1,from_=0,to=5)
		self.scalel1.pack()
		self.scalel1.place(x=210,y=220)

		self.timeshiftl2=Label(self.root)
		self.timeshiftl2.pack()
		self.timeshiftl2.place(x=120,y=270)
		self.timeshiftl2.config(text ="Time Shift")

		self.selectionl2=''
		self.l2=0
		self.scalel2=Scale(self.root,orient='horizontal',length=200,command=self.print_valuel2,from_=-1.0,to=1.0,tickinterval=0.5,resolution=0.5)
		self.scalel2.pack()
		self.scalel2.place(x=210,y=260)

		self.timescalel3=Label(self.root)
		self.timescalel3.pack()
		self.timescalel3.place(x=120,y=320)
		self.timescalel3.config(text ="Time Scale")

		self.selectionl3=''
		self.l3=0
		self.scalel3=Scale(self.root,orient='horizontal',command=self.print_valuel3,from_=0,to=8,length=300,tickinterval=0.125,resolution=0.125)
		self.scalel3.pack()
		self.scalel3.place(x=210,y=320)

		self.var1= IntVar()
		self.sl1=0
		self.var1.trace_variable('w', self.show1)
		self.Cl1 = Checkbutton(self.root, text = "time reversal",onvalue=1,offvalue=0, variable =self.var1)
		self.Cl1.pack()
		self.Cl1.place(x=150,y=375)

		self.var2= IntVar()
		self.sl2=0
		self.var2.trace_variable('w', self.show2)
		self.Cl2 = Checkbutton(self.root, text = "select for modulation",onvalue=1,offvalue=0, variable =self.var2)
		self.Cl2.pack()
		self.Cl2.place(x=150,y=420)


		self.var3= IntVar()
		self.sl3=0
		self.var3.trace_variable('w', self.show3)
		self.Cl3 = Checkbutton(self.root, text = "select for mixing",onvalue=1,offvalue=0, variable =self.var3)
		self.Cl3.pack()
		self.Cl3.place(x=150,y=470)

###################################


		self.buttonbrm = Button(self.root, text="Select File", command=self.showm1)
		self.buttonbrm.pack()
		self.buttonbrm.place(x=470,y=170)
		self.browsem1=Label(self.root)
		self.browsem1.pack()
		self.browsem1.place(x=570,y=170)

		self.wave2=Label(self.root)
		self.wave2.pack()
		self.wave2.place(x=470,y=120)
		self.wave2.config(text ="WAVE 2")

		self.labelampm=Label(self.root)
		self.labelampm.pack()
		self.labelampm.place(x=470,y=220)
		self.labelampm.config(text ="Amplitude")
		self.selectionm1=''
		self.m1=1
		self.scalem1=Scale(self.root,orient='horizontal',command=self.print_valuem1,from_=0,to=5)
		self.scalem1.pack()
		self.scalem1.place(x=570,y=220)

		self.timeshiftm2=Label(self.root)
		self.timeshiftm2.pack()
		self.timeshiftm2.place(x=470,y=270)
		self.timeshiftm2.config(text ="Time Shift")
		self.selectionm2=''
		self.m2=0
		self.scalem2=Scale(self.root,orient='horizontal',command=self.print_valuem2,length=200,from_=-1.0,to=1.0,tickinterval=0.5,     resolution=0.5)
		self.scalem2.pack()
		self.scalem2.place(x=570,y=260)

		self.timescalem3=Label(self.root)
		self.timescalem3.pack()
		self.timescalem3.place(x=470,y=320)
		self.timescalem3.config(text ="Time Scale")
		self.selectionm3=''
		self.m3=0
		self.scalem3=Scale(self.root,orient='horizontal',command=self.print_valuem3,from_=0.0,to=8.0,length=300,tickinterval=0.125,resolution=0.125)
		self.scalem3.pack()
		self.scalem3.place(x=570,y=320)

		self.var4= IntVar()
		self.sm1=0
		self.var4.trace_variable('w',self.show4)
		self.Cl4 = Checkbutton(self.root, text = "time reversal",onvalue=1,offvalue=0, variable =self.var4)
		self.Cl4.pack()
		self.Cl4.place(x=470,y=375)

		self.var5= IntVar()
		self.sm2=0
		self.var5.trace_variable('w', self.show5)
		self.Cl5 = Checkbutton(self.root, text = "select for modulation",onvalue=1,offvalue=0, variable =self.var5)
		self.Cl5.pack()
		self.Cl5.place(x=470,y=420)


		self.var6= IntVar()
		self.sm3=0
		self.var6.trace_variable('w', self.show6)
		self.Cl6 = Checkbutton(self.root, text = "select for mixing",onvalue=1,offvalue=0, variable =self.var6)
		self.Cl6.pack()
		self.Cl6.place(x=470,y=470)

		self.f2=0


#################################################



		self.buttonbrr = Button(self.root, text="Select File", command=self.showr1)
		self.buttonbrr.pack()
		self.buttonbrr.place(x=820,y=170)
		self.browser1=Label(self.root)
		self.browser1.pack()
		self.browser1.place(x=920,y=170)

		self.wave3=Label(self.root)
		self.wave3.pack()
		self.wave3.place(x=820,y=120)
		self.wave3.config(text ="WAVE 3")

		self.labelampr=Label(self.root)
		self.labelampr.pack()
		self.labelampr.place(x=820,y=220)
		self.labelampr.config(text ="Amplitude")
		self.selectionr1=''
		self.r1=1
		self.scaler1=Scale(self.root,orient='horizontal',command=self.print_valuer1,from_=0,to=5)
		self.scaler1.pack()
		self.scaler1.place(x=910,y=220)

		self.timeshiftr2=Label(self.root)
		self.timeshiftr2.pack()
		self.timeshiftr2.place(x=820,y=270)
		self.timeshiftr2.config(text ="Time Shift")
		self.selectionr2=''
		self.r2=0
		self.scaler2=Scale(self.root,orient='horizontal',command=self.print_valuer2,length=200,from_=-1.0,to=1.0,tickinterval=0.5,     resolution=0.51)
		self.scaler2.pack()
		self.scaler2.place(x=910,y=260)

		self.timescaler3=Label(self.root)
		self.timescaler3.pack()
		self.timescaler3.place(x=820,y=320)
		self.timescaler3.config(text ="Time Scale")
		self.selectionr3=''
		self.r3=0
		self.scaler3=Scale(self.root,orient='horizontal',command=self.print_valuer3,from_=0,to=8,length=300,tickinterval=0.5,resolution=0.5)
		self.scaler3.pack()
		self.scaler3.place(x=910,y=320)

		self.var7= IntVar()
		self.sr1=0
		self.var7.trace_variable('w', self.show7)
		self.Cr1 = Checkbutton(self.root, text = "time reversal",onvalue=1,offvalue=0, variable =self.var7)
		self.Cr1.pack()
		self.Cr1.place(x=820,y=375)

		self.var8= IntVar()
		self.sr2=0
		self.var8.trace_variable('w', self.show8)
		self.Cr2 = Checkbutton(self.root, text = "select for modulation",onvalue=1,offvalue=0, variable =self.var8)
		self.Cr2.pack()
		self.Cr2.place(x=820,y=420)


		self.var9= IntVar()
		self.sr3=0
		self.var9.trace_variable('w', self.show9)
		self.Cr3 = Checkbutton(self.root, text = "select for mixing",onvalue=1,offvalue=0, variable =self.var9)
		self.Cr3.pack()
		self.Cr3.place(x=820,y=470)





		self.f1=0



		self.buttonl5 = Button(self.root, text="Play", command=self.getvalue1)
		self.buttonl5.pack()
		self.buttonl5.place(x=120,y=520)
		self.buttonl6 = Button(self.root, text="Pause", command=self.getpausevalue1)
		self.buttonl6.pack()
		self.buttonl6.place(x=220,y=520)
		self.buttonl7 = Button(self.root, text="Stop", command=self.getstopvalue1)
		self.buttonl7.pack()
		self.buttonl7.place(x=320,y=520)
		
		self.buttonm5 = Button(self.root, text="Play", command=self.getvalue2)
		self.buttonm5.pack()
		self.buttonm5.place(x=470,y=520)
		self.buttonm6 = Button(self.root, text="Pause", command=self.getpausevalue2)
		self.buttonm6.pack()
		self.buttonm6.place(x=570,y=520)
		self.buttonm7 = Button(self.root, text="Stop", command=self.getstopvalue2)
		self.buttonm7.pack()
		self.buttonm7.place(x=670,y=520)


		self.buttonr5 = Button(self.root, text="Play", command=self.getvalue3)
		self.buttonr5.pack()
		self.buttonr5.place(x=820,y=520)
		self.buttonr6 = Button(self.root, text="Pause", command=self.getpausevalue3)
		self.buttonr6.pack()
		self.buttonr6.place(x=920,y=520)
		self.buttonr7 = Button(self.root, text="Stop", command=self.getstopvalue3)
		self.buttonr7.pack()
		self.buttonr7.place(x=1020,y=520)

		self.buttonmodulate = Button(self.root, text="Modulate & Play", command=self.modulate)
		self.buttonmodulate.pack()
		self.buttonmodulate.place(x=220,y=670)

		self.buttonmix = Button(self.root, text="Mix & Play", command=self.mix)
		self.buttonmix.pack()
		self.buttonmix.place(x=520,y=670)
		self.value=0
		
		self.rec = Button(self.root, text="Record", command=self.readytorecord)
		self.rec.pack()
		self.rec.place(x=820,y=670)

		
		
		self.pausevalue=' '

		self.pausevalue1=0
		self.pausevalue2=0
		self.pausevalue3=0
		self.stopvalue1=0
		self.stopvalue2=0
		self.stopvalue3=0


	def readytorecord(self):
		r=RECORD()
		r.record_to_file("recorded.wav")

	def modulate(self):
		global mod
		mod=[None]*3
		f1=0
		f2=0
		f3=0
		if self.sl2:
			if len(mod)>0:
				for i in range(len(mod)):
					if mod[i]==filename1:
						f1=1
				if f1==0:
					mod[0]=filename1
					a[0]=float(self.l1)
					tsc[0]=float(self.l2)
					tsh[0]=float(self.l3)
					r[0]=float(self.sl1)
			else:
				mod[0]=(filename1)
				a[0]=float(self.l1)
				tsc[0]=float(self.l2)
				tsh[0]=float(self.l3)
				r[0]=float(self.sl1)

		if self.sm2:
			if len(mod)>0:
				for i in range(len(mod)):
					if mod[i]==filename2:
						f2=1
				if f2==0:
					mod[1]=(filename2)
					a[1]=float(self.m1)
					tsc[1]=float(self.m2)
					tsh[1]=float(self.m3)
					r[1]=float(self.sm1)

			else:
				mod[1]=filename2
				a[1]=float(self.m1)
				tsc[1]=float(self.m2)
				tsh[1]=float(self.m3)
				r[1]=float(self.sm1)
		if self.sr2:
			if len(mod)>0:
				for i in range(len(mod)):
					if mod[i]==filename3:
						f3=1
				if f3==0:
					mod[2]=(filename3)
					a[2]=float(self.r1)
					tsc[2]=float(self.r2)
					tsh[2]=float(self.r3)
					r[2]=float(self.sr1)
			else:
				mod[2]=(filename1)
				a[2]=float(self.r1)
				tsc[2]=float(self.r2)
				tsh[2]=float(self.r3)
				r[2]=float(self.sr1)
		modfunc()
	def mix(self):
		global mix
		mix=[None]*3
		f1=0
		f2=0
		f3=0

		if self.sl3:
			if len(mix)>0:
				for i in range(len(mix)):
					if mix[i]==filename1:
						f1=1
				if f1==0:
					mix[0]=filename1
					a[0]=float(self.l1)
					tsc[0]=float(self.l2)
					tsh[0]=float(self.l3)
					r[0]=float(self.sl1)
			else:
				mix[0]=(filename1)
				a[0]=float(self.l1)
				tsc[0]=float(self.l2)
				tsh[0]=float(self.l3)
				r[0]=float(self.sl1)

		if self.sm3:
			if len(mix)>0:
				for i in range(len(mix)):
					if mix[i]==filename2:
						f2=1
				if f2==0:
					mix[1]=filename2
					a[1]=float(self.m1)
					tsc[1]=float(self.m2)
					tsh[1]=float(self.m3)
					r[1]=float(self.sm1)

			else:
				mix[1]=filename2
				a[1]=float(self.m1)
				tsc[1]=float(self.m2)
				tsh[1]=float(self.m3)
				r[1]=float(self.sm1)
		if self.sr3:
			if len(mix)>0:
				for i in range(len(mix)):
					if mix[i]==filename3:
						f3=1
				if f3==0:
					mix[2]=(filename3)
					a[2]=float(self.r1)
					tsc[2]=float(self.r2)
					tsh[2]=float(self.r3)
					r[2]=float(self.sr1)
			else:
				mix[2]=(filename3)
				a[2]=float(self.r1)
				tsc[2]=float(self.r2)
				tsh[2]=float(self.r3)
				r[2]=float(self.sr1)
		mixfunc()

	def showl1(self):
		global filename1
		filename1 = tkFileDialog.askopenfilename(filetypes = [('wave files', '*.wav'), ('text files', '.txt')])
		self.browsel1.config(text=filename1)
		selected[0]=filename1
		return filename1
	def print_valuel1(self,val):
		self.l1=val
	def print_valuel2(self,val):
		self.l2=val
	def print_valuel3(self,val):
		self.l3=val
	def show1(self,*args):
		self.sl1=self.var1.get()
	def show2(self,*args):
		self.sl2=self.var2.get()
	def show3(self,*args):
		self.sl3=self.var3.get()
	def showm1(self):
		global filename2
		filename2 = tkFileDialog.askopenfilename()
		selected[1]=filename2
		self.browsem1.config(text=filename2)
		return filename2
	def print_valuem2(self,val):
		self.m2=val
	def print_valuem1(self,val):
		self.m1=val
	def print_valuem3(self,val):
		self.m3=val
	def show4(self,*args):
		self.sm1=self.var4.get()
	def show5(self,*args):
		self.sm2=self.var5.get()
	def show6(self,*args):
		self.sm3=self.var6.get()
	def showr1(self):
		global filename3
		filename3 = tkFileDialog.askopenfilename()
		selected[2]=filename3
		self.browser1.config(text=filename3)
		return filename3
	def print_valuer2(self,val):
		self.r2=val
	def print_valuer1(self,val):
		self.r1=val
	def show7(self,*args):
		self.sr1=self.var7.get()
	def print_valuer3(self,val):
		self.r3=val
	def show8(self,*args):
		self.sr2=self.var8.get()
	def show9(self,*args):
		self.sr3=self.var9.get()
	def getvalue1(self):
		self.value=0
		self.play1(self.value)
	def getpausevalue1(self):
		self.pausevalue=0
		self.pause1(self.pausevalue)
	def getpausevalue2(self):
		self.pausevalue=1
		self.pause1(self.pausevalue)
	def getpausevalue3(self):
		self.pausevalue=2
		self.pause1(self.pausevalue)
	def getstopvalue1(self):
		self.stopvalue=0
		self.stop1(self.stopvalue)
	def getstopvalue2(self):
		self.stopvalue=1
		self.stop1(self.stopvalue)
	def getstopvalue3(self):
		self.stopvalue=2
		self.stop1(self.stopvalue)
	def getvalue2(self):
		self.value=1
		self.play1(self.value)
	def getvalue3(self):
		self.value=2
		self.play1(self.value)

	def play1(self,value):
		s2=float(self.l2)
		s3=float(self.l3)
		mix=[]
		modulate=[]
		if value==0:
			file1=mixer(selected[value])

			amp=float(self.l1)

			file1.amp1(amp)

			timeshiftby=s2

			file1.shift(timeshiftby)
			self.pausevalue=0
			self.stopvalue=0
			scaleby=s3
			file1.scale(scaleby)
			if self.sl1:
				file1.reverse1()

			file1.write1('output1.wav')
			self.playf('output1.wav')

		elif value==1:
			file2=mixer(selected[value])
			amp=float(self.m1)
			file2.amp1(amp)
			s5=float(self.m2)
			timeshiftby=s5
			file2.shift(timeshiftby)
			self.pausevalue=1
			self.stopvalue=1
			s6=float(self.m3)
			scaleby=s6
			file2.scale(scaleby)
			if self.sm1:
				file2.reverse1()

			file2.write1('output2.wav')
			self.playf('output2.wav')

		if value==2:
			file3=mixer(selected[value])
			amp=float(self.r1)
			file3.amp1(amp)
			s8=float(self.r2)	
			timeshiftby=s8
			file3.shift(timeshiftby)
			self.pausevalue=2
			self.stopvalue=2
			s9=float(self.r3)
			scaleby=s9
			file3.scale(scaleby)
			if self.sr1:
				file3.reverse1()

			file3.write1('output3.wav')
			self.playf('output3.wav')

	def stop1(self,value):
		if value==0:
			if self.playing1==1 or self.pausevalue==0:
				os.kill(self.pid1,9)
		if value==1:
			if self.playing2==1 or self.pausevalue==1:
				os.kill(self.pid2,9)
		if value==2:
			if self.playing3==1 or self.pausevalue==2:
				os.kill(self.pid3,9)

	def pause1(self,value):
		if value==0:
			if self.playing1==1:
				os.kill(self.pid1,signal.SIGSTOP)
				self.paused1=1
				self.playing1=0
		if value==1:
			if self.playing2==1:
				os.kill(self.pid2,signal.SIGSTOP)
				self.paused2=1
				self.playing2=0
		if value==2:
			if self.playing3==1:
				os.kill(self.pid3,signal.SIGSTOP)
				self.paused3=1
				self.playing3=0



	def playf(self,out):
		played=0.0
		if self.pausevalue==0:
			if self.paused1==1:
				os.kill(self.pid1,signal.SIGCONT)
				self.playing1=1
				self.paused1=0
			else:
				self.playing1=1
				Chunk=1024
				self.pid1=os.fork()
				if self.pid1==0:
					f = wave.open(out,'rb')  
					pl = pyaudio.PyAudio()  
					stream = pl.open(format = pl.get_format_from_width(f.getsampwidth()),channels = f.getnchannels(),rate = f.getframerate(), output = True)  
					data = f.readframes(Chunk)  
					while data != '':  
						stream.write(data)  
						data = f.readframes(Chunk)  
					stream.stop_stream()  
					stream.close()  
					pl.terminate()
					exit(0)
		if self.pausevalue==1:
			if self.paused2==1:
				os.kill(self.pid2,signal.SIGCONT)
				self.playing2=1
				self.paused2=0
			else:
				self.playing2=1
				Chunk=1024
				self.pid2=os.fork()
				if self.pid2==0:
					f = wave.open(out,'rb')  
					pl = pyaudio.PyAudio()  
					stream = pl.open(format = pl.get_format_from_width(f.getsampwidth()),channels = f.getnchannels(),rate = f.getframerate(), output = True)  
					data = f.readframes(Chunk)  
		
					while data != '':  
						stream.write(data)  
						data = f.readframes(Chunk)  
					stream.stop_stream()  
					stream.close()  
					pl.terminate()
					exit(0)
		if self.pausevalue==2:
			if self.paused3==1:
				os.kill(self.pid3,signal.SIGCONT)
				self.playing3=1
				self.paused3=0
			else:
				self.playing3=1
				Chunk=1024
				self.pid3=os.fork()
				if self.pid3==0:
					f = wave.open(out,'rb')  
					pl = pyaudio.PyAudio()  
					stream = pl.open(format = pl.get_format_from_width(f.getsampwidth()),channels = f.getnchannels(),rate = f.getframerate(), output = True)  
					data = f.readframes(Chunk)  
		
					while data != '':  
						stream.write(data)  
						data = f.readframes(Chunk)  
					stream.stop_stream()  
					stream.close()  
					pl.terminate()
					exit(0)



	def main(self):
		self.root.mainloop()





if __name__=="__main__":
	gui=setup()
	gui.main()
#################################################################################
