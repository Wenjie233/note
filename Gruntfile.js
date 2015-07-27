'use strict'

module.exports = function(grunt) {
	grunt.initConfig({
		config: {
			css: 'app/css/'
		},
		less: {
			dev: {
				options: {

					yuicompress: false
				},
				files: [{
					expand: true,
					cwd: '<%= config.css %>',
					src: ['*.less'],
					dest: '<%= config.css %>',
					ext: '.css',
					extDot: 'first'
				}]
			}
		},

		watch: {
			less: {
				files: ['<%= config.css %>/*.less', '<%= config.css %>/*/*.less'],
				tasks: ['less:dev'],
				options: {
					debounceDelay: 250
				}
			}
		}
	});

	grunt.loadNpmTasks('grunt-contrib-watch');
	grunt.loadNpmTasks('grunt-contrib-less');

}
