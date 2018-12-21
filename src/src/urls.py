"""src URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/2.1/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.conf.urls import url, include
from django.contrib import admin
from django.urls import path

urlpatterns = [
    path('admin/', admin.site.urls),
    url(r'^office/', include('office.urls'))
]

# from django.conf.urls import url, include
# from django.http import JsonResponse
# from django.apps import apps
#
#
# urlpatterns = [
#     url(r'^v1/accounts/', include('accounts.urls')),
#     url(r'^v1/documents/', include('documents.urls')),
# ]
# if apps.is_installed('trainset'):
#     urlpatterns.append(url(r'^v1/trainset/', include('trainset.urls', namespace='v1')))
#
#
# def handler404(request):
#     return JsonResponse({'success': False, 'error': 404, 'message': 'Method not found'})