#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul  5 16:48:35 2018

@author: ludovictheobald
"""
import matplotlib
matplotlib.use('MacOSX')
import matplotlib.pyplot as plt
import numpy as np


fp_cuckoo =[]
fp_bloom =[]
fp_simdbloom =[]

time_cuckoo =[]
time_bloom =[]
time_simdbloom =[]

memory_cuckoo =[]
memory_bloom =[]
memory_simdbloom =[]

nb_items =[]
print("debut")

bloom_log = open("bloom_log.txt")
lines=bloom_log.readlines()
i=0
print (lines)
while(i < len(lines)-1):

    if(lines[i]=="Cuckoo \n"):
        nb_items.append(lines[i-1].replace('\n',''))
        memory_cuckoo.append(lines[i+1].replace('\n',''))
        time_cuckoo.append(lines[i+2].replace('\n',''))
        fp_cuckoo.append(lines[i+3].replace('\n',''))
        
        memory_bloom.append(lines[i+5].replace('\n',''))
        time_bloom.append(lines[i+6].replace('\n',''))
        fp_bloom.append(lines[i+7].replace('\n',''))
        
        memory_simdbloom.append(lines[i+9].replace('\n',''))
        time_simdbloom.append(lines[i+10].replace('\n',''))
        fp_simdbloom.append(lines[i+11].replace('\n',''))
        
    i+=1
    

time_bloom = np.asarray(time_bloom,dtype=float)*1000
time_simdbloom= np.asarray(time_simdbloom,dtype=float)*1000
time_cuckoo= np.asarray(time_cuckoo, dtype=float)*1000

fp_bloom = np.asarray(fp_bloom, dtype=float)
fp_simdbloom = np.asarray(fp_simdbloom, dtype=float)
fp_cuckoo = np.asarray(fp_cuckoo, dtype=float)

memory_bloom = np.asarray(memory_bloom, dtype=float)
memory_simdbloom = np.asarray(memory_simdbloom, dtype=float)
memory_cuckoo = np.asarray(memory_cuckoo, dtype=float)

nb_items=np.asarray(nb_items, dtype=int)

print(nb_items)

plt.subplot(311)
plt.plot(nb_items, time_bloom, "b")
plt.plot(nb_items, time_simdbloom, "g")
plt.plot(nb_items, time_cuckoo,'r')
plt.legend()
plt.ylabel('Execution time (ms) ')
plt.xlabel('Number of items in the filter')


plt.subplot(312)
plt.plot(nb_items, fp_bloom, "b")
plt.plot(nb_items, fp_simdbloom,"g")
plt.plot(nb_items, fp_cuckoo,'r')
plt.ylabel('False positive rate')

plt.subplot(313)
plt.plot(nb_items, memory_bloom, "b")
plt.plot(nb_items, memory_simdbloom,'g')
plt.plot(nb_items, memory_cuckoo,'r')
plt.ylabel('Memory space')
plt.show()


    