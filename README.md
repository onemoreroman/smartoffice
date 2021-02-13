# smartoffice
1. Accepts requests with sensors data from Arduino and stores to DB (SQLite)
2. Sensors employed: NTC10k, DS18B20 for temperature, DHT11 for humidity and temperature, CCS811 for air quality (tVOC)  
3. Displays sensors data over the last 1/7/30/X days (check out [lykova11.ru](http://lykova11.ru))


# tree
```
├── Arduino                            # Arduino scripts
│   ├── arduino_eth                    # Arduino + Ethernet shield + NTC10k + DHT11 + CCS811
│   ├── arduino_shiald                 # Arduino + WiFi shield ESP8266 (aka shiald) + DS18B20
│   └── libraries
├── README.md
├── requirements                       # pip requirements 
│   └── base.txt
├── src                                # Django project
│   ├── office                         # main Django app 
│   ├── src_nginx_template.conf        # NGINX config template
│   ├── src_uwsgi_template.ini         # UWSGI config template 
```

# install Django app on Ubuntu
```
1. sudo apt install git vim htop gcc build-essential python3-dev python3-pip nginx
2. git clone git@github.com:onemoreroman/smartoffice.git
3. sudo pip install uwsgi
4. cp smartoffice/src/src_uwsgi_template.ini smartoffice/src/uwsgi.ini 
5. crontab -e
   @reboot /usr/local/bin/uwsgi --ini /root/smartoffice/src/uwsgi.ini
6. cp smartoffice/src/src_uwsgi_template.ini smartoffice/src/uwsgi.ini 
7. vim /etc/nginx/nginx.conf and change user from www-data to root
8. uwsgi --ini smartoffice/src/uwsgi.ini
9. cd smartoffice && . venv/bin/activate
10. pip install -r requirements/base.txt
11. cp src/src_nginx_template.conf /etc/nginx/sites-available/mysite_nginx.conf
12. vim /etc/nginx/sites-available/mysite_nginx.conf and put in your IP instead of 111.111.111.111
13. sudo /etc/init.d/nginx start 
14. sudo ln -s /etc/nginx/sites-available/mysite_nginx.conf /etc/nginx/sites-enabled/
15. cd src && ./manage.py collectstatic
16. crontab -e
    add commands:
    */6 * * * * cd /root/smartoffice/src && /root/smartoffice/venv/bin/python manage.py put_data_to_csv 0 --sample_mins 1
    20 */1 * * * cd /root/smartoffice/src && /root/smartoffice/venv/bin/python manage.py put_data_to_csv 1 --sample_mins 1
    30 0 * * * cd /root/smartoffice/src && /root/smartoffice/venv/bin/python manage.py put_data_to_csv 7 --sample_mins 1
    10 0 * * * cd /root/smartoffice/src && /root/smartoffice/venv/bin/python manage.py put_data_to_csv 30 --sample_mins 2
    0 0 */5 * * cd /root/smartoffice/src && /root/smartoffice/venv/bin/python manage.py put_data_to_csv 365 --sample_mins 10

* if anything goes wrong, try looking at NGINX log: /var/log/nginx/error.log
```
