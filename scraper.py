from selenium import webdriver
from bs4 import BeautifulSoup
from selenium.webdriver.support.wait import WebDriverWait
import pandas as pd
import numpy as np
from time import sleep
import os
dir_path = os.path.realpath(__file__)
def get_km(html_source):
    first = True
    ind1 = 0
    ind2 = 0
    count = 0
    for i in html_source:
        if(i==' ' and first):
            ind1=count 
            first = False
        if(i=='k'):
            ind2=count
        count+=1
    out = html_source[ind1:ind2]
    return(out)


website = 'https://www.openstreetmap.org/directions'
path = '/home/martin/chromedriver_linux64/chromedriver'

driver = webdriver.Chrome(path)
driver.get(website)

locations = ['Barcelona, spain','Madrid, spain','Sevilla, spain','Granada, spain','Paris, france','Genova, italy']
n = len(locations)
distances = np.zeros([n,n])

for i in range(len(locations)):
    for j in np.arange(i+1,len(locations)):


        text_area1 = WebDriverWait(driver, 1).until(
            lambda x: x.find_element('xpath','/html/body/div/div[1]/div[1]/form[2]/div[2]/div[2]/input'))
        text_area1.send_keys(locations[i])

        text_area2 = WebDriverWait(driver, 1).until(
            lambda x: x.find_element('xpath','/html/body/div/div[1]/div[1]/form[2]/div[3]/div[2]/input'))
        text_area2.send_keys(locations[j])
        sleep(0.005*i+0.5)

        search = WebDriverWait(driver, 1).until(
            lambda x: x.find_element('xpath','/html/body/div/div[1]/div[1]/form[2]/div[4]/div[2]/input'))
        search.click()
        sleep(0.005*j+0.6)

        source = WebDriverWait(driver, 2).until(
            lambda x: x.find_element('xpath','/html/body/div/div[1]/div[5]/p[1]'))

        source = source.get_attribute('outerHTML')

        #text = text.find_element('xpath','.//*[@id="sidebar_content"]/p[1]').text

        soup = str(BeautifulSoup(source, 'html.parser').p)
        distance = get_km(soup)
        distances[i,j] = int(distance)
        distances[j,i] = int(distance)
        print(distances)
        text_area1.clear()
        text_area2.clear()

    
##################################
# Create data file
##################################


distances = distances.astype('int64')
n_city = f'''param N:= {n};\n'''

index = f'{np.arange(1,n+1)}'
index = index[1:(len(index)-1)]

dist_matrix = f'''param C: {index}:=\n'''

content = f'{n_city}{dist_matrix}'

for i in range(n):
    dist = f'{distances[i,:]}'
    dist = dist[1:(len(dist)-1)]
    if(i<(n-1)):
        dist = f'{i+1} {dist}\n'
    else:
        dist = f'{i+1} {dist};\n'
    content = f'{content}{dist}'

with open('tsp.dat', 'w') as f:
    f.write(content)



# Connect to neos server
website = 'https://neos-server.org/neos/solvers/lp:CPLEX/AMPL.html'
mail = 'mar1fer1per@gmail.com'
driver.get(website)


text_area1 = WebDriverWait(driver, 1).until(
    lambda x: x.find_element('xpath','/html/body/div[1]/form/div[2]/div[2]/div[1]/div/input'))
text_area1.send_keys(f'{dir_path[0:(len(dir_path)-7)]}/tsp.mod')
sleep(0.1)
text_area2 = WebDriverWait(driver, 1).until(
    lambda x: x.find_element('xpath','/html/body/div[1]/form/div[2]/div[2]/div[2]/div/input'))
text_area2.send_keys(f'{dir_path[0:(len(dir_path)-7)]}/tsp.dat')
sleep(0.2)
text_area3 = WebDriverWait(driver, 1).until(
    lambda x: x.find_element('xpath','/html/body/div[1]/form/div[2]/div[2]/div[3]/div/input'))
text_area3.send_keys(f'{dir_path[0:(len(dir_path)-7)]}/tsp.run')
sleep(0.3)
text_area4 = WebDriverWait(driver, 1).until(
    lambda x: x.find_element('xpath','/html/body/div[1]/form/div[2]/div[2]/div[5]/input[3]'))
text_area4.send_keys(mail)
sleep(0.5)
search = WebDriverWait(driver, 1).until(
    lambda x: x.find_element('xpath','/html/body/div[1]/form/div[2]/div[2]/center/input[1]'))
search.click()