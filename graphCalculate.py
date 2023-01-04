# importing the required libraries
import matplotlib.pyplot as plt
import numpy as np

# define data values


#findtopk.c dosyasında 2,3,5 degerleri için hesaplanan CPU çalışma süreleri
x = np.array([2,3,5]) # X-axis points N = 2 degeri için 
y = np.array([0.001363,0.001202, 0.001788]) # Y-axis points
plt.plot(x, y) # Plot the chart



#findtopk_thread.c dosyasında 2,3,5 degerleri için hesaplanan CPU çalışma süreleri
a = np.array([2,3,5]) # X-axis points N = 3 degeri için
b = np.array([0.004377,0.010298, 0.016876]) # Y-axis points
plt.Color = 'red'
plt.plot(a, b) # Plot the chart



#findtopk_mqueue.c dosyasında 2,3,5 degerleri için hesaplanan CPU çalışma süreleri
c = np.array([2,3,5]) # X-axis points N = 4 degeri için
d = np.array([0.0027,0.0113 , 0.02032]) # Y-axis points

plt.plot(c, d) # Plot the chart



#findtopk.c intermated dosyasıyla haberleşirler ,
#findtopk_thread.c global değişkenlerle haberleşirler,
#findtopk_mqueue.c message queue ile haberleşirler


plt.title('CPU Time Measuring') # Chart title
plt.xlabel('N') # X-axis label
plt.ylabel('CPU Time') # Y-axis label
plt.legend(['findtopk', 'findtopk_thread', 'findtopk_mqueue'], loc='upper left')
plt.show() # display

