from django.contrib import admin
from django.urls import path
from rest_framework.urlpatterns import format_suffix_patterns

from . import views

app_name = 'cisternas'
urlpatterns = [
    path('', views.cisternas_list, name='list'),
    path('<slug:cisterna_slug>', views.cisternas_detail, name='detail'),
    path('teste_api/', views.cis_list),
    path('teste_api/<int:pk>/', views.cis_detail)
    # path('teste_api/(?P<pk>[0-9]+)/$', views.cis_detail)
]

urlpatterns = format_suffix_patterns(urlpatterns)