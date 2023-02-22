# Traveling salesman problem with selenium scraper

In this project was developed a scraper bot using selenium. This bot automatize the route search process between each pair in a set of locations using Open Street Map (https://www.openstreetmap.org). The bot also write the correspondent TSP optimization problem using the Miller, Tucker, and Zemlin formulation (https://dl.acm.org/doi/10.1145/321043.321046) in AMPL to send the problem to the NEOS server and get the solution. Its also provided a simple colony ant optimization implementation to get rasonable good solutions without using the NEOS server (https://neos-server.org/neos/).

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

The scraper will use Open Street Maps to extract the distances between each pair of locations and write this distance matrix as a AMPL set of parameters in the tsp.dat file. When the scraping process is finalised the AMPL files will be sent to the NEOS server with your email. You wil recive a report when the solver finds the solution.
