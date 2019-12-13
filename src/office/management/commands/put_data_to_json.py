from datetime import datetime, timedelta, timezone

from django.core.management.base import BaseCommand, CommandError

import pandas as pd

from office.models import SensorsData, Sensors


class Command(BaseCommand):
    help = 'Put sensors data to CSV'

    def add_arguments(self, parser):
        parser.add_argument('delta_days', type=int)
        parser.add_argument('sample_mins', type=int)

    def handle(self, *args, **options):
        sensors = Sensors.objects.all()

        dt0 = datetime.now(tz=timezone.utc) - timedelta(days=options['delta_days'])
        freq = str(options['sample_mins']) + 'T'
        if options['delta_days'] < 3:
            time_format = '%a %H:%M'
        elif options['delta_days'] < 31:
            time_format = '%d %H:%M'
        elif options['delta_days'] < 366:
            time_format = '%b %d %H'
        else:
            time_format = '%Y-%m-%d'

        tss = []
        for i, sensor in enumerate(sensors):
            q = SensorsData.objects.filter(sensor=sensor, time__gte=dt0)
            time = [_.time for _ in q]
            vals = [float(_.value) for _ in q]
            ts = pd.Series(vals, index=pd.to_datetime(time))
            ts = ts.resample(freq).mean()
            tss.append(ts)

        dt_min = min([ts.index[0] for ts in tss])
        dt_max = max([ts.index[-1] for ts in tss])

        df = pd.DataFrame(index=pd.date_range(dt_min, dt_max, freq=freq))
        df['local_time'] = df.index.tz_convert('Asia/Krasnoyarsk').strftime(time_format)
        for i, sensor in enumerate(sensors):
            df[sensor.name] = tss[i]
        df.to_csv(str(options['delta_days'])+'days_'+str(options['sample_mins'])+'mins.csv', index=False)
