from django.db import models


class Sensors(models.Model):
    name = models.CharField(max_length=128)
    location = models.CharField(max_length=128, default='')
    type = models.IntegerField(default=0)
    units = models.CharField(default='', max_length=16, null=True, blank=True)


class SensorsData(models.Model):
    sensor = models.ForeignKey('office.Sensors', on_delete=models.CASCADE)
    value = models.CharField(default='', max_length=64, null=True, blank=True)
    time = models.DateTimeField(auto_now_add=True)
