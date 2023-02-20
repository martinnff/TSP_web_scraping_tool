from selenium import webdriver
from bs4 import BeautifulSoup
from selenium.webdriver.support.wait import WebDriverWait
import pandas as pd
import numpy as np
from time import sleep


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



