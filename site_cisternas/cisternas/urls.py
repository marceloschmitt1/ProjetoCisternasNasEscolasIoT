from django.contrib import admin
from django.urls import path

from . import views

app_name = 'cisternas'
urlpatterns = [
    path('', views.cisternas_list, name='list'),
    path('<slug:cisterna_slug>', views.cisternas_detail, name='detail'),
    path('teste_api/', views.cis_list)
]