from django.urls import path

from . import views

# https://bitsofpy.blogspot.com/2009/07/matplotlib-in-django.html
#   from django.conf.urls.defaults import *

app_name = 'polls'
urlpatterns = [
    # ex: /polls/
    path('', views.index, name='index'),
    # ex: /polls/5/
    # detail(request=<HttpRequest object>, question_id=34)
    path('details/<int:question_id>/', views.detail, name='detail'),
    # ex: /polls/5/results/
    path('<int:question_id>/results/', views.results, name='results'),
    # ex: /polls/5/vote/
    path('<int:question_id>/vote/', views.vote, name='vote'),


    # path('<int:question_id>/vote/', views.vote_on_choice, name='choice_vote')
    path('<int:question_id>/graph_results', views.graph_results, name='graph_results')
]