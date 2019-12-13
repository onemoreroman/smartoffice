from django.conf.urls import url
from django.urls import path
from rest_framework.urlpatterns import format_suffix_patterns

from office import views


urlpatterns = [
    url(r'upload_data$', views.upload_data, name='upload_data'),
    path('info/', views.index, name='info'),
]

urlpatterns = format_suffix_patterns(urlpatterns)
