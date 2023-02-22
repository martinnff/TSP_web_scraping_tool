# Traveling salesman problem with selenium scraper

In this project was developed a scraper bot using selenium. This bot automatize the route search process between each pair in a set of locations using Open Street Maps. The bot also write the correspondent TSP optimization problem using the Miller, Tucker, and Zemlin formulation in AMPL to send the problem to the neos server and get the solution. Its also provided a simple colony ant optimization implementation to get rasonable good solutions without using the NEOS server.

### Usage


Download, unzip the repository and edit the config.txt file. The first line should contain the email were you want to get sent your results, the second line should have the path to the chrome webdriver and the folowing lines the search terms for each location.

```
'your_mail@mail.com'
'/path/to/chromedriver'
'Barcelona, spain'
'Madrid, spain'
'Sevilla, spain'
'Granada, spain'
```

Once the config file is correcltly writen simply run the scraper file.

```{python}
python3 scraper.py
```
