from datetime import datetime, timedelta, timezone
import uuid

from django.core.management.base import BaseCommand

import numpy as np

from sensor.models import SensorData, Sensor


class Command(BaseCommand):
    help = 'Generate random data in DB'

    def add_arguments(self, parser):
        parser.add_argument('--delta_days', type=int, default=0)
        parser.add_argument('--samples', type=int, default=100)

    def handle(self, *args, **options):
        sensor_name = 'test_sensor_'+uuid.uuid4().hex[:3]
        sensor = Sensor.objects.create(name=sensor_name, display_name=sensor_name)
        dt1 = datetime.now(tz=timezone.utc)
        if options['delta_days'] == 0:
            dt0 = dt1 - timedelta(hours=1)
        else:
            dt0 = dt1 - timedelta(days=options['delta_days'])

        N = options['samples']
        random_time = np.random.rand(N)
        random_time.sort()
        random_time = datetime.timestamp(dt0) + (datetime.timestamp(dt1) - datetime.timestamp(dt0)) * random_time
        random_time = [datetime.fromtimestamp(t, tz=timezone.utc) for t in random_time]
        random_data = 100.0 * np.random.rand(N)
        data_to_db = [SensorData(sensor=sensor, time=t, value=v) for (t, v) in zip(random_time, random_data)]
        SensorData.objects.bulk_create(data_to_db)
        print(f'Created sensor {sensor.name} with {N} data points')