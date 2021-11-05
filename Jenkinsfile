pipeline {
    agent {
        label 'docker-amt'
    }
    options {
        buildDiscarder(logRotator(numToKeepStr: '5', daysToKeepStr: '30'))
        timestamps()
        timeout(unit: 'HOURS', time: 2)
    }
    
    
    stages {
        stage ('Cloning Repository') {
            steps {
                script {
                    scmCheckout {
                        clean = true
                    }
                }
            }
        }
        stage('Static Code Scan - Protex') {
            environment{
                PROJECT_NAME               = 'OpenAMT - RPC'
                SCANNERS                   = 'protex'
            }
            when {
                anyOf {
                    branch 'main';
                }
            }
            steps {
                rbheStaticCodeScan()
            }
        }
        stage ('Parallel Builds') {
            parallel {
                stage ('Linux') {
                    agent { label 'docker-amt' }
                    stages {
                        stage('Build') {
                            agent {
                                docker {
                                    image 'ubuntu:18.04'
                                    reuseNode true
                                }
                            }
                            steps {
                                sh './scripts/jenkins-pre-build.sh'
                                sh './scripts/jenkins-build.sh'
                                stash includes: 'build/rpc', name: 'linux-rpc-app'
                            }
                        }
                        stage ('Archive') {
                            steps {
                                archiveArtifacts allowEmptyArchive: true, artifacts: 'build/rpc', caseSensitive: false, onlyIfSuccessful: true
                            }
                        }
                    }
                }
                stage ('Windows') {
                    agent { label 'openamt-win' }
                    stages{
                        stage ('Build') {
                            steps {
                                bat 'scripts\\jenkins-pre-build.cmd'
                                bat 'scripts\\jenkins-build.cmd'
                                // prepare stash for the binary scan
                                stash includes: '**/*.exe', name: 'win-rpc-app'
                            }
                        }
                        stage ('Archive') {
                            steps {
                                archiveArtifacts allowEmptyArchive: true, artifacts: 'build\\Release\\rpc.exe', caseSensitive: false, onlyIfSuccessful: true
                            }
                        }
                    }
                }
            }
        }

        stage('Prep Binary') {
            steps {
                sh 'mkdir -p ./bin'
                dir('./bin') {
                    unstash 'linux-rpc-app'
                    unstash 'win-rpc-app'
                }
            }
        }
        stage('Linux Scans') {
            environment{
                PROJECT_NAME               = 'OpenAMT - RPC - Linux'
                SCANNERS                   = 'bdba,klocwork'

                // protecode details
                PROTECODE_BIN_DIR          = './bin'
                PROTECODE_INCLUDE_SUB_DIRS = true
                
                // klocwork details
                KLOCWORK_SCAN_TYPE             = 'c++'
                KLOCWORK_PRE_BUILD_SCRIPT      = './scripts/jenkins-pre-build.sh'
                KLOCWORK_BUILD_COMMAND         = './scripts/jenkins-build.sh'
                KLOCWORK_IGNORE_COMPILE_ERRORS = true

                // publishArtifacts details
                PUBLISH_TO_ARTIFACTORY         = true
            }
            steps {
                rbheStaticCodeScan()
                dir('artifacts/Klockwork'){
                    sh 'cp kw_report.html kw_report_linux.html'
                    sh 'cp kw_report.csv kw_report_linux.csv'
                    archiveArtifacts allowEmptyArchive: true, artifacts: 'kw_report_linux.html'
                    archiveArtifacts allowEmptyArchive: true, artifacts: 'kw_report_linux.csv'
                }
                
            }
        }
        stage('Windows Scans'){
            agent { label 'openamt-win' }
            stages{
                stage ('Windows Scans - klocwork') {
                    environment {
                        PROJECT_NAME                   = 'OpenAMT - RPC - Windows'
                        SCANNERS                       = 'klocwork'

                        // klocwork details
                        KLOCWORK_SCAN_TYPE             = 'c++'
                        KLOCWORK_PRE_BUILD_SCRIPT      = 'scripts\\jenkins-pre-build.cmd'
                        KLOCWORK_BUILD_COMMAND         = 'scripts\\jenkins-build.cmd'
                        KLOCWORK_IGNORE_COMPILE_ERRORS = true

                        // publishArtifacts details
                        PUBLISH_TO_ARTIFACTORY         = true
                    }
                    steps {
                        rbheStaticCodeScan()
                        dir('artifacts\\Klockwork'){
                            bat 'copy kw_report.html kw_report_windows.html'
                            bat 'copy kw_report.csv kw_report_windows.csv'
                            stash includes: 'kw_report_windows.*', name: 'win-kwreports'
                            archiveArtifacts allowEmptyArchive: true, artifacts: 'kw_report_windows.html'
                            archiveArtifacts allowEmptyArchive: true, artifacts: 'kw_report_windows.csv'
                        }
                    }
                }
            }
        }
        stage('Publish Artifacts'){
            steps{
                dir('artifacts/Klockwork'){
                    unstash 'win-kwreports'
                }
                publishArtifacts()
            }
        }
    }
}
