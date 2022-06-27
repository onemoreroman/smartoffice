from pms7003 import Pms7003Sensor, PmsSensorException
import time

from django.core.management.base import BaseCommand

from sensor.models import SensorData, Sensor


class Command(BaseCommand):
    help = 'Read data from PMS7003 connected to /dev/ttyS0'

    def add_arguments(self, parser):
        parser.add_argument('delta_secs', type=int)

    def handle(self, *args, **options):
        sensor_pm1_0 = Sensor.objects.filter('pm1_0').first()
        sensor_pm2_5 = Sensor.objects.filter('pm2_5').first()
        sensor_pm10 = Sensor.objects.filter('pm10').first()
        while True:
            try:
                pms_sensor = Pms7003Sensor('/dev/ttyS0')
                pms_sensor_data = pms_sensor.read()
                pm1_0 = pms_sensor_data['pm1_0']
                pm2_5 = pms_sensor_data['pm2_5']
                pm10 = pms_sensor_data['pm10']
                sdata = SensorData(sensor=sensor_pm1_0)
                sdata.value = pm1_0
                sdata.save()
                sdata = SensorData(sensor=sensor_pm2_5)
                sdata.value = pm2_5
                sdata.save()
                sdata = SensorData(sensor=sensor_pm10)
                sdata.value = pm10
                sdata.save()
                time.sleep(options['delta_secs'])
            except PmsSensorException:
                print('Connection problem')
