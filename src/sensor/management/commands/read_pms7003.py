from pms7003 import Pms7003Sensor
from datetime import datetime, timezone
import time
from retry import retry

from django.core.management.base import BaseCommand

from sensor.models import SensorData, Sensor


class Command(BaseCommand):
    help = 'Read data from PMS7003 connected to /dev/ttyS0'

    @retry(tries=3, delay=2)
    def handle(self, *args, **options):
        sensor_pm1_0 = Sensor.objects.filter(name='PMS7003_PM1_0').first()
        sensor_pm2_5 = Sensor.objects.filter(name='PMS7003_PM2_5').first()
        sensor_pm10 = Sensor.objects.filter(name='PMS7003_PM10').first()
        t = datetime.now(timezone.utc)
        pms = Pms7003Sensor('/dev/ttyS0')
        pms_data = pms.read()
        pm1_0 = pms_data['pm1_0']
        pm2_5 = pms_data['pm2_5']
        pm10 = pms_data['pm10']
        sdata = SensorData(sensor=sensor_pm1_0)
        sdata.value = pm1_0
        sdata.time = t
        sdata.save()
        sdata = SensorData(sensor=sensor_pm2_5)
        sdata.value = pm2_5
        sdata.time = t
        sdata.save()
        sdata = SensorData(sensor=sensor_pm10)
        sdata.value = pm10
        sdata.time = t
        sdata.save()
